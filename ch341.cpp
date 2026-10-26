#include "ch341.h"
#include "CH341DLL.H"
#include "qdebug.h"
#include "mainwindow.h"
#include <QMessageBox>
extern MainWindow * w_p;
extern CH341 ch341;
CH341::CH341(QObject *parent) : QObject(parent), deviceIndex(0), hDevice(INVALID_HANDLE_VALUE)
{
    //intInit();              // INT无法触发回调函数，取消初始化
    setDeviceNotify();      // 这函数不需要插上CH341时调用
    i2cSpeed = I2C_SPEED_20KHz;
    chipSelectPin = CS0;
    xSB = MSB_First;
}
CH341::~CH341()
{
    closeDevice();
    cancelDeviceNotify();
}
// 回调函数
// 回调函数必须是全局函数或者静态成员函数
void CALLBACK CH341DeviceEvent(ULONG iEventStatus)
{
    if (iEventStatus == 0)
    {
        qDebug()<<"设备拔出";
        ch341.closeDevice();
        w_p->setStatusText(" × 设备拔出，设备未连接");
        //w_p->enableOpenButton(true);
    }
    else if (iEventStatus == 3)
    {
        qDebug()<<"设备插入";
        ch341.openDevice();
        w_p->setStatusText(" √ 设备插入，设备已连接");
        //w_p->enableOpenButton(false);
    }
    else if (iEventStatus == 1)
    {
        qDebug()<<"设备将要拔出";
    }
    else
    {
        qDebug()<<"设备其他";
    }
}
// 中断回调函数,需要在里面添加程序
void CALLBACK intCallback(ULONG iStatus)
{
    qDebug()<<"中断触发了"<<iStatus;
}

bool CH341::setDeviceNotify()
{
    return CH341SetDeviceNotify(deviceIndex,NULL,CH341DeviceEvent);
}
void CH341::cancelDeviceNotify()
{
    CH341SetDeviceNotify(deviceIndex, nullptr, nullptr);
}
// 打开设备成功或失败后默认会打印消息
bool CH341::openDevice(ULONG index)
{
    if(openState == DEVICE_OPEN)
    {
        return true;
    }
    HANDLE hDevice = CH341OpenDevice(index);
    if (hDevice == INVALID_HANDLE_VALUE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device open failed");
        openState = DEVICE_CLOSE;
        return false;
    }
    QString ch341Name = QString::fromLocal8Bit((const char*)CH341GetDeviceName(index));

    // 设置超时和独占模式
    CH341SetTimeout(index, 3000, 3000);               // 读写超时3秒
    CH341SetExclusive(index, TRUE);                   // 独占设备
    qDebug()<<"CH341 device opened successfully";     // 成功
    qDebug()<<"devicename:"<<ch341Name;               // 设备名称
    qDebug()<<"IC verson:"<<((CH341GetVerIC(index)-0x10)?"CH341A":"CH341");   // 设备型号CH341或CH341A
    openState = DEVICE_OPEN;
    i2cSetSpeed(i2cSpeed);
    spiInit(chipSelectPin,xSB);
    return true;
}
// 打开设备成功或失败后默认会打印消息
void CH341::closeDevice(ULONG index)
{
    CH341CloseDevice(index);
    qDebug()<<"CH341 device closed";
    openState = DEVICE_CLOSE;
}
bool CH341::resetDevice()
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    return CH341ResetDevice(deviceIndex);
}
bool CH341::delayMs(int xms)              // 用于IO口操作延时
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    return CH341SetDelaymS(deviceIndex,xms);
}


// 成功或失败后不会打印消息,但是会返回状态标志
bool CH341::i2cWriteByte(UCHAR regAddr,UCHAR writeData)
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    return CH341WriteI2C(deviceIndex, salveAddr, regAddr, writeData);
}
// 成功或失败后不会打印消息,但是会返回状态标志
bool CH341::i2cReadByte(UCHAR regAddr)
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    return CH341ReadI2C(deviceIndex, salveAddr, regAddr, &i2cReadData);
}
// 设置速度函数
void CH341::i2cSetSpeed(I2CSpeed i2cSpeed)
{
    this->i2cSpeed = i2cSpeed;
    CH341SetStream(deviceIndex,(xSB|i2cSpeed));
}
bool CH341::i2cReadBytes(UCHAR regAddr,ULONG readLenth,UCHAR * buffer)
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    i2cWriteBuffer[0] = (salveAddr<< 1) & 0xFE;
    i2cWriteBuffer[1] = regAddr;
    return CH341StreamI2C(deviceIndex,2,i2cWriteBuffer,readLenth,buffer);
}
void CH341::spiInit(CSPin pin,ByteFirst SB)
{

    chipSelect = pin;
    chipSelect |= 0x80;                             // 使能片选控制,必须要使能,否则SPI无法正常工作(试验出来如果不使能MOSI无法发送数据)
                                                    // 这意味着一定要硬件上选择CS,而不能软件选择CS(其实也可以,不能用D0~D7的IO口作为软件片选引脚)
    xSB = SB;                                       // 默认高位在前
    CH341SetStream(deviceIndex,(xSB|i2cSpeed));     // 默认SPI数据高位在前，四线SPI
}
bool CH341::spiSwapBytes(ULONG lenth,UCHAR *swapBuffer)
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    return CH341StreamSPI4(deviceIndex,chipSelect,lenth,swapBuffer);
}

void CH341::GPIO_Init(GPIO_PIN pinName,IO_DIR dir)
{
    if(dir == OUTPUT)       // 如果设置输出模式的话一开始默认输出低电平
    {
        outputBits |= pinName;
        GPIO_SetBits(dataBits);
    }
    else if(dir == INPUT)   // 输入模式一定是默认外部上拉的
    {
        outputBits &= (~pinName);
        GPIO_SetBits(dataBits);
    }
}
bool CH341::GPIO_SetBits(ULONG outputData)
{
    ULONG enableData=0x1F;
    dataBits = outputData;
    // 位0为1说明iSetDataOut的位15-位8有效,否则忽略
    // 位1为1说明iSetDirOut的位15-位8有效,否则忽略
    // 位2为1说明iSetDataOut的7-位0有效,否则忽略
    // 位3为1说明iSetDirOut的位7-位0有效,否则忽略
    // 位4为1说明iSetDataOut的位23-位16有效,否则忽略
    // 这里使能所有的引脚

    return CH341SetOutput(deviceIndex,enableData,outputBits,dataBits);    // 设置CH341的I/O方向,并通过CH341直接输出数据
}

bool CH341::GPIO_SetDataBits(GPIO_PIN pinName,GPIO_STATE ioState)
{
    if(ioState == GPIO_HIGH)
    {
        dataBits |= pinName;
    }
    else if(ioState == GPIO_LOW)
    {
        dataBits &= (~pinName);
    }
    return GPIO_SetBits(dataBits);
}
ULONG CH341::GPIO_ReadBits()
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    CH341GetStatus(deviceIndex,&iStatus);
    return iStatus;
}
bool CH341::GPIO_ReadDataBits(GPIO_PIN pin)
{
    if (openState == DEVICE_CLOSE)          // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    GPIO_ReadBits();
    if((iStatus & pin))
    {
        return 1;   //返回高电平
    }
    else
    {
        return 0;   //返回低电平
    }
}
// 中断初始化,INT引脚默认上拉
// 当CH341的INT#引脚出现上升沿时，DLL自动调用iIntRoutine，并向其提供一个引脚状态参数，
// 引脚状态参数中，位为1则说明对应的引脚为高电平，位为0则说明对应的引脚为低电平，
// 位7-位0对应CH341的D7-D0引脚，位8对应CH341的ERR#引脚，位9对应CH341的PEMP引脚，
// 位10对应CH341的INT#引脚，位11对应CH341的SLCT引脚
// 但是经过试验这个函数无法相应
bool CH341::intInit()
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    return CH341SetIntRoutine(deviceIndex,intCallback);
}

bool CH341::intDeInit()
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    return CH341SetIntRoutine(deviceIndex,NULL);
}

// 读取INT所有引脚的状态
bool CH341::readInter()
{
    if (openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        qWarning("CH341 device not opened");
        return false;
    }
    return CH341ReadInter(deviceIndex,&iStatus);
}

