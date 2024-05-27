#include "mainwindow.h"
#include "qgridlayout.h"
#include "qheaderview.h"
#include "qlabel.h"
#include "qpushbutton.h"
//#include "qtablewidget.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("onlinestudy");
    //QVBoxLayout *vboxlayout = new QVBoxLayout();

    //QTabWidget *
    tabWidget = new QTabWidget(this);
    //tabWidget->setFixedSize(width(), height());
    tabWidget->resize(width(), height());
    //tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setStyleSheet("QTabBar::tab { height: 30px; width: 100px; }");

    QWidget *widgetfx = new QWidget;//(this);
    QWidget *widgetzx = new QWidget;
    this->setCentralWidget(widgetfx);
    tabWidget->addTab(widgetfx, "法宣学习");
    tabWidget->addTab(widgetzx, "干部在线");
/////////////////////////////////////////////////////////////////
    //QTextEdit
    textEditFx=new QTextEdit(widgetfx);
    textEditFx->setReadOnly(true);
    QFont textEditFont ( "Microsoft YaHei", 9, 50);
    textEditFx->setFont(textEditFont);

    //QLabel *
    labelFx = new QLabel(widgetfx);
    //label->setText("QLabel");
    labelFx->setAlignment(Qt::AlignCenter);//居中显示
    pushButtonFx = new QPushButton(widgetfx);
    pushButtonFx->setText("开 始");
    pushButtonFx->setFixedSize(80,23);
    //QTableWidget
    tableWidgetFx = new QTableWidget(widgetfx);
    tableWidgetFx->setShowGrid(true); //设置显示格子线
    tableWidgetFx->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    tableWidgetFx->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
    tableWidgetFx->setRowCount(1); //设置行数
    tableWidgetFx->setColumnCount(3); //设置列数
    //tableWidget->setWindowTitle("TABLE演示");
    //tableWidgetFx->horizontalHeader()->setStretchLastSection(true);
    tableWidgetFx->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
    //tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    tableWidgetFx->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    QStringList header;
    header<<"编 号"<<"标 题"<<"状 态";
    tableWidgetFx->setHorizontalHeaderLabels(header);
    //去掉默认行号 可以用horizontalHeader() ->setVisible(false)隐藏横向表头
    //tableWidget->verticalHeader()->setVisible(false);

    QGridLayout *gridlayoutfx = new QGridLayout(widgetfx);
    gridlayoutfx->addWidget(labelFx,0,0,1,1);//将QLabel添加到网格布局的0行0列，占用1行1列
    gridlayoutfx->addWidget(pushButtonFx,0,2,1,1);
    gridlayoutfx->addWidget(tableWidgetFx,1,0,15,3);
    gridlayoutfx->addWidget(textEditFx,16,0,5,3);
    widgetfx->setLayout(gridlayoutfx);
//////////////////////////////////////////////////////////////////////////////////
    //QTextEdit
    textEditZx=new QTextEdit(widgetzx);
    textEditZx->setReadOnly(true);
    QFont textEditFontZx ( "Microsoft YaHei", 9, 50);
    textEditFx->setFont(textEditFontZx);

    //QLabel *
    labelZx = new QLabel(widgetzx);
    //label->setText("QLabel");
    labelZx->setAlignment(Qt::AlignCenter);//居中显示
    pushButtonZx = new QPushButton(widgetzx);
    pushButtonZx->setText("开 始");
    pushButtonZx->setFixedSize(80,23);
    //QTableWidget
    tableWidgetZx = new QTableWidget(widgetzx);
    tableWidgetZx->setShowGrid(true); //设置显示格子线
    tableWidgetZx->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    tableWidgetZx->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
    tableWidgetZx->setRowCount(1); //设置行数
    tableWidgetZx->setColumnCount(3); //设置列数
    //tableWidget->setWindowTitle("TABLE演示");
    tableWidgetZx->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
    //tableWidgetZx->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    tableWidgetZx->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    QStringList header1;
    header1<<"编 号"<<"标 题"<<"状 态";
    tableWidgetFx->setHorizontalHeaderLabels(header1);

    QStringList header2;
    header2<<"编 号"<<"标 题"<<"时 长";
    tableWidgetZx->setHorizontalHeaderLabels(header2);
    //去掉默认行号 可以用horizontalHeader() ->setVisible(false)隐藏横向表头
    //tableWidget->verticalHeader()->setVisible(false);

    QGridLayout *gridlayoutzx = new QGridLayout(widgetzx);
    gridlayoutzx->addWidget(labelZx,0,0,1,1);//将QLabel添加到网格布局的0行0列，占用1行1列
    gridlayoutzx->addWidget(pushButtonZx,0,2,1,1);
    gridlayoutzx->addWidget(tableWidgetZx,1,0,15,3);
    gridlayoutzx->addWidget(textEditZx,16,0,5,3);
    widgetzx->setLayout(gridlayoutzx);
//////////////////////////////////////////////////////////////////////////////////////
    //vboxlayout->addWidget(tabWidget);

    //TestAes();
    threadclientfx=new ThreadRunClient();
    qthreadfx=new QThread();
    threadclientfx->moveToThread(qthreadfx);

    threadclientzx=new ThreadOnline();
    qthreadzx=new QThread();
    threadclientzx->moveToThread(qthreadzx);
    //
    connect(threadclientfx,&ThreadRunClient::returnLog,this,&MainWindow::updateTextFx);
    connect(threadclientzx,&ThreadOnline::returnLog,this,&MainWindow::updateTextZx);
    //
    connect(threadclientfx,&ThreadRunClient::returnResult,this,&MainWindow::updateTableFx);
    connect(threadclientzx,&ThreadOnline::returnResult,this,&MainWindow::updateTableZx);
    //线程结束时清理线程内存
    connect(qthreadfx, &QThread::finished, qthreadfx, &QThread::deleteLater);
    connect(qthreadzx, &QThread::finished, qthreadzx, &QThread::deleteLater);
    //将按钮事件（信号）绑定槽函数
    connect(pushButtonFx, &QPushButton::clicked, threadclientfx, &ThreadRunClient::runClient);
    connect(pushButtonFx, &QPushButton::clicked, this, &MainWindow::onButtonFxClicked);
    connect(pushButtonZx, &QPushButton::clicked, threadclientzx, &ThreadOnline::runClientOnline);
    connect(pushButtonZx, &QPushButton::clicked, this, &MainWindow::onButtonZxClicked);
    //关闭窗口的时候结束线程
    //connect(this,SIGNAL(destroyed()),this,SLOT(quitThreadSlot()));
    qthreadfx->start();
    qthreadzx->start();
}
//重载resizeevent,改变窗口大小时处理事件
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    tabWidget->resize(width(), height());//随窗口改变大小
}
void MainWindow::onButtonFxClicked()
{
    pushButtonFx->setDisabled(true);
}
void MainWindow::updateTableFx(QVariant dataVal,QString totalPeriod)
{
    QVector<QVector<QString>> array;

    for(int row = tableWidgetFx->rowCount() - 1;row >= 0; row--)
    {
        tableWidgetFx->removeRow(row);
    }

    array = dataVal.value<QVector<QVector<QString>>>();
    for(int i=0;i<array.size();i++)
    {
        tableWidgetFx->insertRow(i);
        tableWidgetFx->setItem(i,0,new QTableWidgetItem(array[i][0]));
        tableWidgetFx->setItem(i,1,new QTableWidgetItem(array[i][1]));
        tableWidgetFx->setItem(i,2,new QTableWidgetItem(array[i][2]));
    }
    tableWidgetFx->update();
    labelFx->setText("totalPeriod:"+totalPeriod);

}
void MainWindow::updateTextFx(QString text)
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间

    QString strTime = dateTime .toString("yyyy-MM-dd hh:mm:ss");//格式化时
    QString log=strTime+": "+text+'\n';
    QTextCursor cursor=textEditFx->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEditFx->setTextCursor(cursor);
    textEditFx->insertPlainText(log);
}
/////////////////////////////////////////////////////////////////////////////
void MainWindow::updateTableZx(QVariant dataVal,QString totalPeriod)
{
    QVector<QVector<QString>> array;

    for(int row = tableWidgetZx->rowCount() - 1;row >= 0; row--)
    {
        tableWidgetZx->removeRow(row);
    }

    array = dataVal.value<QVector<QVector<QString>>>();
    for(int i=0;i<array.size();i++)
    {
        tableWidgetZx->insertRow(i);
        tableWidgetZx->setItem(i,0,new QTableWidgetItem(array[i][0]));
        tableWidgetZx->setItem(i,1,new QTableWidgetItem(array[i][1]));
        tableWidgetZx->setItem(i,2,new QTableWidgetItem(array[i][2]));
    }
    tableWidgetZx->update();
    labelZx->setText(totalPeriod);
}
void MainWindow::updateTextZx(QString text)
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间

    QString strTime = dateTime .toString("yyyy-MM-dd hh:mm:ss");//格式化时
    QString log=strTime+": "+text+'\n';
    QTextCursor cursor=textEditZx->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEditZx->setTextCursor(cursor);
    textEditZx->insertPlainText(log);
}
void MainWindow::onButtonZxClicked()
{
    pushButtonZx->setDisabled(true);
}
void MainWindow::TestAes()
{
    //QString inputStr="aaaaasdfsdfsdfsdfaaaaeter6364365a";
    QString timestamp = "1713142637034";//QString::number(QDateTime::currentMSecsSinceEpoch());
    QString usercount=";4228110010016";
    QString rid=";df42e14e8db54813b81b508a0f2ac181;PC";
    QString input=timestamp+usercount+rid;
    QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray key = QString("5KLwKRd7tjsEQZmM").toUtf8();
    QByteArray iv = QString("erVtH2aehRE8WrTE").toUtf8();

    QByteArray encodeText = encryption.encode(input.toUtf8(), key, iv);
    QString encodeStr = encodeText.toBase64();
    qDebug() << "encodeText:" << encodeText;
    qDebug() << "encodeStr:" << encodeStr;
    QLocale locale = QLocale::English;
    QString format = "ddd MMMM dd yyyy hh:mm:ss";
    QString currentTime =locale.toString(QDateTime::currentDateTimeUtc(), format) + " GMT+0800";
    qDebug()<<currentTime;

    std::string str;
    str="\"12345\"";
    QByteArray stdByte = QByteArray::fromStdString(str);
    QNetworkCookie cy("loginUser",stdByte) ;
    QList<QNetworkCookie> allcookies;
    allcookies.append(cy);


    qDebug()<<stdByte;
}
MainWindow::~MainWindow()
{
    delete ui;
    //qthreadfx->quit();
   // qthreadfx->wait();
    //qthreadzx->quit();
    //qthreadzx->wait();
}
