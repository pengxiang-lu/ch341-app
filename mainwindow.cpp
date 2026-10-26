#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QIcon"
#include "QLabel"
#include "ch341.h"
#include "QMessageBox"
#include "qdebug.h"
#include "QDesktopServices"
CH341 ch341;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("CH341编程器");
    QIcon icon(":/Image/chip.png");
    setWindowIcon(icon);
    ui->statusbar->showMessage(" × 设备未连接");
    //enableOpenButton(true);

    // 表格初始化
    QStringList IO_Mode = {"高阻","输出","输入"}; // 下拉选项
    QStringList IO_State = {"", ""}; // 下拉选项

    for (int i = 0;i < 8; i++)
    {
        QComboBox *combo1 = new QComboBox(this);
        combo1->addItems(IO_Mode);                 // 添加选项
        QComboBox *combo2 = new QComboBox(this);
        combo2->addItems(IO_State);                // 添加选项
        // 将下拉框嵌入单元格（第row行，第1列）
        ui->tableWidget_IO1->setCellWidget(0,i, combo1);
        ui->tableWidget_IO1->setCellWidget(1,i, combo2);
        ui->tableWidget_IO1->cellWidget(1,i)->setEnabled(false);
        QTableWidgetItem* statusItem = new QTableWidgetItem("");  // 创建空文本项
        ui->tableWidget_IO1->setItem(2, i, statusItem);  // 关联到单元格
        connect(combo1, &QComboBox::currentIndexChanged, this,
        [=]()
        {
            if(qobject_cast<QComboBox*>(ui->tableWidget_IO1->cellWidget(0,i))->currentText() == "输出")
            {
                ui->tableWidget_IO1->cellWidget(1,i)->setEnabled(true);
                combo2->setItemText(0,"高电平");
                combo2->setItemText(1,"低电平");
            }
            else
            {
                ui->tableWidget_IO1->cellWidget(1,i)->setEnabled(false);
                combo2->setItemText(0,"");
                combo2->setItemText(1,"");
            }
        });
    }
    for (int i = 0;i < 7; i++)
    {
        QComboBox *combo1 = new QComboBox(this);
        combo1->addItems(IO_Mode);                 // 添加选项
        QComboBox *combo2 = new QComboBox(this);
        combo2->addItems(IO_State);                // 添加选项
        // 将下拉框嵌入单元格（第row行，第1列）
        ui->tableWidget_IO2->setCellWidget(0,i, combo1);
        ui->tableWidget_IO2->setCellWidget(1,i, combo2);
        ui->tableWidget_IO2->cellWidget(1,i)->setEnabled(false);
        QTableWidgetItem* statusItem = new QTableWidgetItem("");  // 创建空文本项
        ui->tableWidget_IO2->setItem(2, i, statusItem);  // 关联到单元格
        connect(combo1, &QComboBox::currentIndexChanged, this,
                [=]()
                {
                    if(qobject_cast<QComboBox*>(ui->tableWidget_IO2->cellWidget(0,i))->currentText() == "输出")
                    {
                        ui->tableWidget_IO2->cellWidget(1,i)->setEnabled(true);
                        combo2->setItemText(0,"高电平");
                        combo2->setItemText(1,"低电平");
                    }
                    else
                    {
                        ui->tableWidget_IO2->cellWidget(1,i)->setEnabled(false);
                        combo2->setItemText(0,"");
                        combo2->setItemText(1,"");
                    }
                });
    }
    // 后面四个输出
    for (int i = 7;i < 11; i++)
    {
        QComboBox *combo1 = new QComboBox(this);
        combo1->addItems(QStringList("高阻"));      // 添加选项
        combo1->addItems(QStringList("输出"));
        QComboBox *combo2 = new QComboBox(this);
        combo2->addItems(IO_State);                // 添加选项
        // 将下拉框嵌入单元格（第row行，第1列）
        ui->tableWidget_IO2->setCellWidget(0,i, combo1);
        ui->tableWidget_IO2->setCellWidget(1,i, combo2);
        ui->tableWidget_IO2->cellWidget(1,i)->setEnabled(false);
        QTableWidgetItem* statusItem = new QTableWidgetItem("");    // 创建空文本项
        ui->tableWidget_IO2->setItem(2, i, statusItem);             // 关联到单元格
        connect(combo1, &QComboBox::currentIndexChanged, this,
                [=]()
                {
                    if(qobject_cast<QComboBox*>(ui->tableWidget_IO2->cellWidget(0,i))->currentText() == "输出")
                    {
                        ui->tableWidget_IO2->cellWidget(1,i)->setEnabled(true);
                        combo2->setItemText(0,"高电平");
                        combo2->setItemText(1,"低电平");
                    }
                    else
                    {
                        ui->tableWidget_IO2->cellWidget(1,i)->setEnabled(false);
                        combo2->setItemText(0,"");
                        combo2->setItemText(1,"");
                    }
                });
    }
    // -------- 连接信号与槽 --------
    // actionIIC 触发时，调用 onActionIICTriggered
    connect(ui->actionIIC, &QAction::triggered, this, &MainWindow::onActionIICTriggered);
    // actionSPI 触发时，调用 onActionSPITriggered
    connect(ui->actionSPI, &QAction::triggered, this, &MainWindow::onActionSPITriggered);
    // actionCH341 触发时，调用 onActionCH341Triggered
    connect(ui->actionCH341, &QAction::triggered, this, &MainWindow::onActionCH341Triggered);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_open_clicked()
{
    if (ch341.openState == DEVICE_OPEN)        // 此时代表没有设备连接
    {
        ui->statusbar->showMessage(" √ 设备已连接");
        return ;
    }
    if(ch341.openDevice()==true)
    {
        ui->statusbar->showMessage(" √ 设备已连接");
    }
    else
    {
        ui->statusbar->showMessage(" × 设备打开失败");
    }
}

void MainWindow::on_pushButton_close_clicked()
{
    if (ch341.openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        ui->statusbar->showMessage(" × 设备已断开");
        return ;
    }
    ch341.closeDevice();
    ui->statusbar->showMessage(" × 设备已断开");
}

void MainWindow::on_pushButton_IICWriteByte_clicked()
{
    ch341.i2cWriteData = ui->spinBox_IICWriteByte->value();
    if (ch341.openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        QMessageBox::critical(this, "错误", "设备未打开");
    }
    else if(ch341.i2cWriteByte(ch341.regAddr,ch341.i2cWriteData)==0)
    {
        //QMessageBox::critical(this, "错误", "写入失败");
        //ui->statusbar->showMessage("I2C写入失败");
    }
    else
    {
        ui->statusbar->showMessage("I2C写入成功");
    }
}

void MainWindow::on_spinBox_SalveAddr_valueChanged(int arg1)
{
    qDebug()<<arg1;
    ch341.salveAddr = ui->spinBox_SalveAddr->value();
}


void MainWindow::on_radioButton_s1_clicked()
{
    ch341.i2cSpeed = CH341::I2C_SPEED_20KHz;
    ch341.i2cSetSpeed(ch341.i2cSpeed);
}


void MainWindow::on_radioButton_s2_clicked()
{
    ch341.i2cSpeed = CH341::I2C_SPEED_100KHz;
    ch341.i2cSetSpeed(ch341.i2cSpeed);
}


void MainWindow::on_radioButton_s3_clicked()
{
    ch341.i2cSpeed = CH341::I2C_SPEED_400KHz;
    ch341.i2cSetSpeed(ch341.i2cSpeed);
}


void MainWindow::on_radioButton_s4_clicked()
{
    ch341.i2cSpeed = CH341::I2C_SPEED_750KHz;
    ch341.i2cSetSpeed(ch341.i2cSpeed);
}


void MainWindow::on_pushButton_IICRead_clicked()
{
    if (ch341.openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        QMessageBox::critical(this, "错误", "设备未打开");
    }
    else if(ch341.i2cReadByte(ch341.regAddr) == 0)
    {
        QMessageBox::critical(this, "错误", "读取失败");
        ui->statusbar->showMessage("I2C读取失败");
    }
    else
    {
        ui->lineEdit_IICRead->setText("0x"+QString::number(ch341.i2cReadData,16));
        ui->statusbar->showMessage("I2C读取成功");
    }
}


void MainWindow::on_spinBox_RegAddr_valueChanged(int arg1)
{
    qDebug()<<arg1;
    ch341.regAddr = ui->spinBox_RegAddr->value();
}


void MainWindow::on_radioButton_CS0_clicked()
{
    ch341.chipSelectPin = CH341::CS0;
    ch341.spiInit(ch341.chipSelectPin,ch341.xSB);
}


void MainWindow::on_radioButton_CS1_clicked()
{
    ch341.chipSelectPin = CH341::CS1;
    ch341.spiInit(ch341.chipSelectPin,ch341.xSB);
}


void MainWindow::on_radioButton_CS2_clicked()
{
    ch341.chipSelectPin = CH341::CS2;
    ch341.spiInit(ch341.chipSelectPin,ch341.xSB);
}


void MainWindow::on_pushButton_Swap_clicked()
{

    for(int i = 0;i < ui->spinBox_SwapNum->value();i++)
    {
        ch341.spiSwapBuffer[i] = ui->tableWidget_SPI->item(0,i)->text().toUInt(nullptr,16);
    }
    if (ch341.openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        QMessageBox::critical(this, "错误", "设备未打开");
    }
    else if(ch341.spiSwapBytes(ui->spinBox_SwapNum->value(),ch341.spiSwapBuffer)==0)
    {
        QMessageBox::critical(this, "错误", "SPI交换字节错误");
        ui->statusbar->showMessage("SPI交换字节错误");
    }
    else
    {
        for(int i = 0;i < 16;i++)
        {
            ui->tableWidget_SPI->item(1,i)->setText("");
        }
        for(int i = 0;i < ui->spinBox_SwapNum->value();i++)
        {
            ui->tableWidget_SPI->item(1,i)->setText("0x"+QString::number(ch341.spiSwapBuffer[i],16));
        }
        ui->statusbar->showMessage("SPI交换字节成功");
    }
}


void MainWindow::on_radioButton_MSB_clicked()
{
    if (ch341.openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        QMessageBox::critical(this, "错误", "设备未打开");
        return;
    }
    ch341.xSB = CH341::MSB_First;
    ch341.spiInit(ch341.chipSelectPin,ch341.xSB);
}


void MainWindow::on_radioButton_LSB_clicked()
{
    if (ch341.openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        QMessageBox::critical(this, "错误", "设备未打开");
        return;
    }
    ch341.xSB = CH341::LSB_First;
    ch341.spiInit(ch341.chipSelectPin,ch341.xSB);
}


void MainWindow::on_pushButton_IICReadBytes_clicked()
{
    if (ch341.openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        QMessageBox::critical(this, "错误", "设备未打开");
    }
    else if(ch341.i2cReadBytes(ch341.regAddr,ui->spinBox_IICReadNum->value(),ch341.i2cReadBuffer)==0)
    {
        QMessageBox::critical(this, "错误", "I2C读取多字节错误");
        ui->statusbar->showMessage("I2C读取多字节错误");
    }
    else
    {
        for(int i = 0;i < 16;i++)
        {
            ui->tableWidget_IIC->item(0,i)->setText("");
        }
        for(int i = 0;i < ui->spinBox_IICReadNum->value();i++)
        {
            ui->tableWidget_IIC->item(0,i)->setText("0x"+QString::number(ch341.i2cReadBuffer[i],16));
        }
        ui->statusbar->showMessage("I2C读取多字节成功");
    }
}

void MainWindow::on_pushButton_Reflash_clicked()
{
    if (ch341.openState == DEVICE_CLOSE)        // 此时代表没有设备连接
    {
        QMessageBox::critical(this, "错误", "设备未打开");
        return;
    }
    // 遍历
    // 遍历第 0~7 列（假设表格有 8 列，需确保 i 不越界）
    for (int i = 0; i < 8; i++)  // 建议用 tableWidget_IO1->columnCount() 替代固定值 8，避免越界
    {
        if(qobject_cast<QComboBox*>(ui->tableWidget_IO1->cellWidget(0,i))->currentText() == "输入")
        {
            ch341.GPIO_Init(CH341::GPIO_PIN(0x01<<i),CH341::INPUT);
            if(ch341.GPIO_ReadDataBits(CH341::GPIO_PIN(0x01<<i)) == 0)
            {
                ui->tableWidget_IO1->item(2,i)->setText("低电平");
            }
            else
            {
                ui->tableWidget_IO1->item(2,i)->setText("高电平");
            }
        }
        else if(qobject_cast<QComboBox*>(ui->tableWidget_IO1->cellWidget(0,i))->currentText() == "输出")
        {
            ch341.GPIO_Init(CH341::GPIO_PIN(0x01<<i),CH341::OUTPUT);
            if(qobject_cast<QComboBox*>(ui->tableWidget_IO1->cellWidget(1,i))->currentText()=="高电平")
            {
                ch341.GPIO_SetDataBits(CH341::GPIO_PIN(0x01<<i),CH341::GPIO_HIGH);
            }
            else
            {
                ch341.GPIO_SetDataBits(CH341::GPIO_PIN(0x01<<i),CH341::GPIO_LOW);
            }
            ui->tableWidget_IO2->item(2,i)->setText("");
        }
        else
        {
            ch341.GPIO_Init(CH341::GPIO_PIN(0x01<<i),CH341::INPUT);
            ui->tableWidget_IO1->item(2,i)->setText("");
        }
    }
    for (int i = 0; i < 11; i++)
    {
        if(qobject_cast<QComboBox*>(ui->tableWidget_IO2->cellWidget(0,i))->currentText() == "输入")
        {
            ch341.GPIO_Init(CH341::GPIO_PIN(0x01<<ch341.IO_TransTable[i+8]),CH341::INPUT);
            if(ch341.GPIO_ReadDataBits(CH341::GPIO_PIN(0x01<<ch341.IO_TransTable[i+8])) == 0)
            {
                ui->tableWidget_IO2->item(2,i)->setText("低电平");
            }
            else
            {
                ui->tableWidget_IO2->item(2,i)->setText("高电平");
            }
        }
        else if(qobject_cast<QComboBox*>(ui->tableWidget_IO2->cellWidget(0,i))->currentText() == "输出")
        {
            ch341.GPIO_Init(CH341::GPIO_PIN(0x01<<ch341.IO_TransTable[i+8]),CH341::OUTPUT);
            if(qobject_cast<QComboBox*>(ui->tableWidget_IO2->cellWidget(1,i))->currentText()=="高电平")
            {
                ch341.GPIO_SetDataBits(CH341::GPIO_PIN(0x01<<ch341.IO_TransTable[i+8]),CH341::GPIO_HIGH);
            }
            else
            {
                ch341.GPIO_SetDataBits(CH341::GPIO_PIN(0x01<<ch341.IO_TransTable[i+8]),CH341::GPIO_LOW);
            }
            ui->tableWidget_IO2->item(2,i)->setText("");
        }
        else
        {
            ch341.GPIO_Init(CH341::GPIO_PIN(0x01<<i),CH341::INPUT);
            ui->tableWidget_IO2->item(2,i)->setText("");
        }
    }
}
// -------- 槽函数实现：打开 IIC 通信协议 PDF --------
void MainWindow::onActionIICTriggered()
{
    // PDF 相对路径（需替换为实际文件路径）
    QString pdfPath = "pdf/I2C_BUS_PROTOCOL.pdf";
    // 转换为绝对路径（结合应用程序当前目录）

    QString absolutePath = QCoreApplication::applicationDirPath() + "/" + pdfPath;
    // 调用系统默认 PDF 阅读器打开
    QDesktopServices::openUrl(QUrl::fromLocalFile(absolutePath));
}

// -------- 槽函数实现：打开 SPI 通信协议 PDF --------
void MainWindow::onActionSPITriggered()
{
    QString pdfPath = "pdf/SPI Protocol.pdf";
    QString absolutePath = QCoreApplication::applicationDirPath() + "/" + pdfPath;
    QDesktopServices::openUrl(QUrl::fromLocalFile(absolutePath));
}

// -------- 槽函数实现：打开 CH341 手册 PDF --------
void MainWindow::onActionCH341Triggered()
{
    QString pdfPath = "pdf/CH341A原理图.pdf";
    QString absolutePath = QCoreApplication::applicationDirPath() + "/" + pdfPath;
    QDesktopServices::openUrl(QUrl::fromLocalFile(absolutePath));
}


