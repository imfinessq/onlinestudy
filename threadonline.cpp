#include "threadonline.h"

ThreadOnline::ThreadOnline(QObject *parent)
    : QObject{parent}
{}

void ThreadOnline::imageToHex(QImage &image)
{
    int WIDTH = image.width();
    int HEIGHT =image.height();
    uchar *img=image.bits();
    int r,g,b,rgbave;
    for(int i=0;i<WIDTH*HEIGHT;i++)
    {
        r = img[4 * i];
        g = img[4 * i + 1];
        b = img[4 * i + 2];
        rgbave=(r+g+b)/3;
        if(rgbave>120)
        {
            img[4 * i] = 255;
            img[4 * i + 1] = 255;
            img[4 * i + 2] = 255;
        }
        else
        {
            img[4 * i] = 0;
            img[4 * i + 1] = 0;
            img[4 * i + 2] = 0;
        }

    }
}
void ThreadOnline::runClientOnline()
{
    //qDebug()<<"sssssssssss";
    userAccount="42011119761103503x";
    userPassword="8251032ssq.SSQ";
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    getCode();
    connect(timer, SIGNAL(timeout()), this, SLOT(getTimeResetProgress()));
}
void ThreadOnline::getCode()
{
    emit returnLog("login");
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    url = QUrl("http://www.hbgbzx.gov.cn/portal/index!imgcode.action?a="+timestamp);
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);

    //get方式发送请求
    //req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    //qt6后默认自动重定向，设置为手动才能得到重定向以前的数据
    reply = manager->get(req);
    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
    connect(reply,&QNetworkReply::finished,this,&ThreadOnline::replyCode);
}
void ThreadOnline::replyCode()
{

    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray imageData=reply->readAll();
    //qDebug() << "接收到的数据" <<imageData;
    QImage image=QImage::fromData(imageData);
    imageToHex(image);
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    QString ss=QCoreApplication::applicationDirPath();//QDir::currentPath();
    std::string ss1 = ss.toStdString();
    const char* ch = ss1.c_str();
    if (api->Init(ch, "eng"))
    {
        //fprintf(stderr, "Could not initialize tesseract.\n");
        emit returnLog("error code=10");
        //exit(1);
        return;
    }
    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);
    image.save(&buf, "BMP");

    Pix *pix = pixReadMemBmp((const l_uint8*) ba.constData(), ba.size());
    api->SetImage(pix);
    //char *outText;
    QString str;
    //outText= api->GetUTF8Text();
    str=api->GetUTF8Text();
    str.replace(" ","");
    str=str.left(str.size()-1);
    api->End();
    // 数据读取完成之后，清除reply
    qDebug()<<str;

    // <2>检测网页返回状态码，常见是200，404等，200为成功
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QVariant variantCookies = reply->header(QNetworkRequest::SetCookieHeader);
    //QList<QNetworkCookie>
    cookies = qvariant_cast<QList<QNetworkCookie> >(variantCookies);
    cookie=cookies.at(0);
    reply->deleteLater();
    reply = nullptr;

    loginOnline(str);
}
void ThreadOnline::loginOnline(QString code)
{
    QString ss="user_name="+userAccount+"&user_pwd="+userPassword+"&imgCode="+code;
    QByteArray dataArray;
    dataArray=ss.toUtf8();
    url = QUrl("http://www.hbgbzx.gov.cn/portal/login.action");
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    //post方式发送请求
    //QVariant var;
    //var.setValue(cookie);
    //req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::CookieHeader,QVariant::fromValue(cookies));
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作

    connect(reply,&QNetworkReply::finished,this,&ThreadOnline::replyLogin);
}
void ThreadOnline::replyLogin()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    QString str;;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    if(str.indexOf("alert")>=0)
    {
        emit returnLog("login faile");
        getCode();
    }
    else
    {getStudent();}
}
void ThreadOnline::getStudent()
{
    emit returnLog("receive data");
    url = QUrl("http://www.hbgbzx.gov.cn/student/student.action");
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    //get方式发送请求
    req.setHeader(QNetworkRequest::CookieHeader,QVariant::fromValue(cookies));
    reply = manager->get(req);
    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
    connect(reply,&QNetworkReply::finished,this,&ThreadOnline::replyStudent);
}
void ThreadOnline::replyStudent()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    QString str;;
    str=reply->readAll();
    //QString
    lablezx=str.mid(str.indexOf("(")+2,str.indexOf("\\n")-str.indexOf("(")-2);
    reply->deleteLater();
    reply = nullptr;
    getLessonList();
}
void ThreadOnline::getLessonList()
{
    url = QUrl("http://www.hbgbzx.gov.cn/student/course!list.action?course.course_type=3&init=yes");
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    //get方式发送请求
    req.setHeader(QNetworkRequest::CookieHeader,QVariant::fromValue(cookies));
    reply = manager->get(req);
    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
    connect(reply,&QNetworkReply::finished,this,&ThreadOnline::replyLessonList);
}
void ThreadOnline::replyLessonList()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    QString str;;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    QRegExp rxtitlecode("class=\"course_name\"[^<]*<");
    QRegExp rxminutecode("[0-9]+分钟/[0-9]+分钟");
    QRegExp rxtimecode(">[0-9].[0-9]<");
    QRegExp rxidcode("videoList\\([0-9]+\\)");
    QVector<QString> title,minute,studytime,listid;
    int pos=0;
    while ((pos = rxtitlecode.indexIn(str, pos)) != -1)
    {
        pos += rxtitlecode.matchedLength();
        QString kk=rxtitlecode.cap(0);
        kk.replace("class=\"course_name\">","");
        kk.replace("<","");
        title.append(kk);
    }
    pos=0;
    while ((pos = rxminutecode.indexIn(str, pos)) != -1)
    {
        pos += rxminutecode.matchedLength();
        QString kk=rxminutecode.cap(0);

        minute.append(kk);
    }
    pos=0;
    while ((pos = rxtimecode.indexIn(str, pos)) != -1)
    {
        pos += rxtimecode.matchedLength();
        QString kk=rxtimecode.cap(0);
        kk.replace("<","");
        kk.replace(">","");
        studytime.append(kk);
    }
    pos=0;
    while ((pos = rxidcode.indexIn(str, pos)) != -1)
    {
        pos += rxidcode.matchedLength();
        QString kk=rxidcode.cap(0);
        kk.replace("videoList(","");
        kk.replace(")","");
        listid.append(kk);
    }
    QVector<QVector<QString>> tableArray;
    QVector<QString> temp;
    tableArray.clear();
    int length=title.size();
    for(int i=0;i<length;i++)
    {
        temp.append(listid[i]);
        temp.append(title[i]);
        temp.append(minute[i]);
        //temp.append(studytime[i]);
        tableArray.append(temp);
        temp.clear();
    }
    QVariant dataVar;
    dataVar.setValue(tableArray);
    emit returnResult(dataVar,lablezx+"  "+userAccount);
    if(length==0)
    {
        emit returnLog("study finish");
        return;
    }
    playId=listid[0];

    lessonTimeSecond=minute[0].replace(QRegExp("[0-9]+分钟/"),"");
    lessonTimeSecond.replace("分钟","");
    int stime=lessonTimeSecond.toInt();
    stime=stime*60;
    lessonTimeSecond=QString::number(stime);
    getPlayId();

}
void ThreadOnline::getPlayId()
{
    url = QUrl("http://www.hbgbzx.gov.cn/portal/study!play.action?id="+playId);
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    //get方式发送请求
    req.setHeader(QNetworkRequest::CookieHeader,QVariant::fromValue(cookies));
    reply = manager->get(req);
    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
    connect(reply,&QNetworkReply::finished,this,&ThreadOnline::replyPlayId);
}
void ThreadOnline::replyPlayId()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    QString str;;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    QRegExp rxcode("param=[^\"]*");
    rxcode.indexIn(str);
    QString kk=rxcode.cap(0);
    QString strjson=kk.mid(6);

    QByteArray urlDecode = QByteArray::fromPercentEncoding(strjson.toUtf8());
    strjson=QString(urlDecode);
    qDebug()<<strjson;
    QJsonParseError jsonError;
    QJsonValue thisValue;
    QJsonDocument doucment = QJsonDocument::fromJson(strjson.toUtf8(), &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject())
        {  // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            QJsonValue dataValue = object.value("uuid");
            uuid=dataValue.toVariant().toString();
            qDebug()<<uuid;
            dataValue = object.value("ucid");
            ucid=dataValue.toVariant().toString();
            if (object.contains("serialize_sco"))
            {
                QJsonValue dataValue = object.value("serialize_sco");
                QString temp=dataValue.toVariant().toString();
                QRegExp rxtime("[0-9]+.0");
                rxtime.indexIn(temp);
                lessonLocation=rxtime.cap(0);
                lessonLocation.replace(".0","");

            }
            else
            {
                lessonLocation="18";
            }
            timer->start(30000);//定时30秒
            emit returnLog("start qtime");
        }

    }
}
void ThreadOnline::getTimeResetProgress()
{
    int stime=lessonTimeSecond.toInt();
    int locationTime=lessonLocation.toInt();
    locationTime=locationTime+30;
    emit returnLog(playId+"  "+QString::number(locationTime)+" --- "+lessonTimeSecond);
    if(locationTime>=stime+30)
    {
        emit returnLog("stop qtime");
        timer->stop();
        getCode();
    }
    else
    {
        lessonLocation=QString::number(locationTime);
        QString strTime=QDateTime::currentDateTime().toString("yyyy-MM-dd+hh:mm:ss");
        QString strUrl="http://www.hbgbzx.gov.cn/portal/study!seek.action?callback=showData&uuid="+uuid+"&id="+ucid+"&serializeSco="+
                      "{\"0\":{\"cmi.core.exit\":\"logout\",\"cmi.core.lesson_location\":\""+
                      lessonLocation+".0\",\"cmi.core.session_time\":\"00:01:53\",\"cmi.core.entry\":\"resume\",\"last_learn_time\":\""+
                      strTime+"\"},\"last_study_sco\":\"0\"}"+"&duration=30000&a=0";
        url = QUrl(strUrl);
        manager = new QNetworkAccessManager(this);
        req.setUrl(url);
        //get方式发送请求
        req.setHeader(QNetworkRequest::CookieHeader,QVariant::fromValue(cookies));
        reply = manager->get(req);
        //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
        connect(reply,&QNetworkReply::finished,this,&ThreadOnline::replyTimeResetProgress);
    }
}
void ThreadOnline::replyTimeResetProgress()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    QString str;;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
}
