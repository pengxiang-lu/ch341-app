#ifndef CH341_H
#define CH341_H
#include <QObject>
#include "CH341DLL.H"
#include <windows.h>  // 需包含Windows头文件（CH341库基于Windows API）
enum OpenState      // 打开状态: OPEN CLOSE
{
    DEVICE_CLOSE,
    DEVICE_OPEN
};
extern OpenState openState;                            // 开启标志
class CH341 : public QObject
{
    Q_OBJECT
public:
    explicit CH341(QObject *parent = nullptr);
    ~CH341();

    enum I2CSpeed           // I2C接口速度/SCL频率, 00=低速/20KHz,01=标准/100KHz(默认值),10=快速/400KHz,11=高速/750KHz
    {
        I2C_SPEED_20KHz  = 0x00,
        I2C_SPEED_100KHz = 0x01,
        I2C_SPEED_400KHz = 0x10,
        I2C_SPEED_750KHz  = 0x11,
    };
    // SPI片选引脚
    enum CSPin              // SPI模式
    {
        CS0 = 0x00,
        CS1 = 0x01,
        CS2 = 0x02,
    };
    enum ByteFirst          // SPI高位低位模式
    {
        MSB_First = 0x80,
        LSB_First = 0x00,
    };
    enum IO_DIR
    {
        INPUT = 0x00,
        OUTPUT = 0x01
    };
    // CH341的GPIO口设计默认上拉的
    enum GPIO_PIN
    {
        D0  = 0x00000001,
        D1  = 0x00000002,
        D2  = 0x00000004,
        D3  = 0x00000008,
        D4  = 0x00000010,
        D5  = 0x00000020,
        D6  = 0x00000040,
        D7  = 0x00000080,
        ERR = 0x0000100,
        PEMP= 0x00000200,
        INT = 0x00000400,
        SLCT= 0x00000800,
        WAIT= 0x00002000,
        DATAS = 0x00004000,
        ADDRS = 0x00008000,
        RESET = 0x00010000,
        WRITE = 0x00020000,
        SCL   = 0x00040000,
        SDA   = 0x10000000,
    };// 以下引脚只能输出,不考虑I/O方向: 位16对应CH341的RESET#引脚, 位17对应CH341的WRITE#引脚, 位18对应CH341的SCL引脚, 位29对应CH341的SDA引脚
    enum GPIO_STATE
    {
        GPIO_HIGH,
        GPIO_LOW
    };

    // 基础操作函数
    bool openDevice(ULONG index = 0);                    // 打开设备
    void closeDevice(ULONG index = 0);                   // 关闭设备
    bool setDeviceNotify();                                     // 事件设置
    void cancelDeviceNotify();                                  // 取消事件设置
    bool resetDevice();                                         // 复位设备
    bool delayMs(int xms);                                      // 硬件操作延时


    // I2C操作相关
    ULONG salveAddr;                                            // 设备地址(写地址),不需要左移,所以地址是0x68而不是0xD0
    ULONG regAddr;                                              // 寄存器地址
    UCHAR i2cWriteData;                                         // 待写入数据
    UCHAR i2cReadData;                                          // 读取数据缓冲区
    UCHAR i2cWriteBuffer[16];                                   // 写入数据的数组，设置最大16个
    UCHAR i2cReadBuffer[16];                                    // 读取数据的数组，设置最大16个
    I2CSpeed  i2cSpeed;                                         // I2C速度
    bool i2cWriteByte(UCHAR regAddr,UCHAR writeData);           // 写单字节
    bool i2cReadByte(UCHAR regAddr);                            // 读单字节
    bool i2cReadBytes(UCHAR regAddr,ULONG readLenth,UCHAR * buffer); // 读多字节
    void i2cSetSpeed(I2CSpeed i2cSpeed);                        // 设置速度

    // SPI操作相关
    UCHAR spiSwapBuffer[16];                                    // SPI交换数组，最大16
    UCHAR spiSwapData;                                          // 交换数据
    ULONG chipSelect;                                           // CS引脚选择变量
    ByteFirst xSB;                                              // 高位还是低位在前
    CSPin chipSelectPin;                                        // 片选引脚编号
    bool spiSwapBytes(ULONG lenth,UCHAR *swapBuffer);           // 交换一组字节
    void spiInit(CSPin pin,ByteFirst SB);                       // SPI初始化设置

    // IO操作相关
    ULONG outputBits;                                           // 输出数据
    ULONG dataBits;                                             // D7~D0数据
    ULONG IO_TransTable[19] = {0,1,2,3,4,5,6,7,8,9,10,11,13,14,15,16,17,18,29};
    void GPIO_Init(GPIO_PIN pinName,IO_DIR dir);
    bool GPIO_SetDataBits(GPIO_PIN pinName,GPIO_STATE ioState);
    bool GPIO_SetBits(ULONG outputData);
    ULONG GPIO_ReadBits();
    bool GPIO_ReadDataBits(GPIO_PIN pin);

    bool intInit();                                             // 外部中断初始化
    bool intDeInit();                                           // 取消初始化
    bool readInter();                                           // 读取中断引脚数据

    ULONG openState;
    ULONG deviceIndex;                                          // 设备标识
    ULONG iStatus;                                              // 状态标志
    HANDLE hDevice;
};

#endif // CH341_H
