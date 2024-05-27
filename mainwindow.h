#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "qlabel.h"
#include "qpushbutton.h"
#include "qtablewidget.h"
#include "qtextedit.h"
#include "threadrunclient.h"
#include "threadonline.h"
#include "ui_mainwindow.h"
#include "qaesencryption.h"
#include <QVariant>

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
    void TestAes();

private slots:
    //void on_pushButton_ok_clicked();
    void updateTableFx(QVariant dataVal,QString totalPeriod);
    void updateTextFx(QString text);
    void onButtonFxClicked();

    void updateTableZx(QVariant dataVal,QString totalPeriod);
    void updateTextZx(QString text);
    void onButtonZxClicked();

private:
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
    ThreadRunClient* threadclientfx; //实例化ThreadRunClient类
    QThread* qthreadfx;   //实例化QThread类
    ThreadOnline* threadclientzx;
    QThread* qthreadzx;
    QTableWidget *tableWidgetFx;
    QLabel *labelFx;
    QTextEdit *textEditFx;
    QPushButton *pushButtonFx;
    QTableWidget *tableWidgetZx;
    QLabel *labelZx;
    QTextEdit *textEditZx;
    QPushButton *pushButtonZx;
protected:
    void resizeEvent(QResizeEvent *event) override;

};
#endif // MAINWINDOW_H
