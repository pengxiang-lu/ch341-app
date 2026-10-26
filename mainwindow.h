#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // 静态函数：修改状态栏内容
    void setStatusText(const QString& text)
    {
        this->ui->statusbar->showMessage(text);
    }
    void enableOpenButton(bool state)
    {
        // ui->pushButton_open->setEnabled(state);
        // ui->pushButton_close->setEnabled(!state);
    }

private slots:
    void on_pushButton_open_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_IICWriteByte_clicked();

    void on_spinBox_SalveAddr_valueChanged(int arg1);

    void on_radioButton_s1_clicked();

    void on_radioButton_s2_clicked();

    void on_radioButton_s3_clicked();

    void on_radioButton_s4_clicked();

    void on_pushButton_IICRead_clicked();

    void on_spinBox_RegAddr_valueChanged(int arg1);

    void on_radioButton_CS0_clicked();

    void on_radioButton_CS1_clicked();

    void on_radioButton_CS2_clicked();

    void on_pushButton_Swap_clicked();

    void on_radioButton_MSB_clicked();

    void on_radioButton_LSB_clicked();

    void on_pushButton_IICReadBytes_clicked();

    void on_pushButton_Reflash_clicked();

    // 每个 QAction 对应的槽函数
    void onActionIICTriggered();
    void onActionSPITriggered();
    void onActionCH341Triggered();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
