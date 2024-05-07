#include "threadrunclient.h"
#include <QDebug>

ThreadRunClient::ThreadRunClient(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<QVariant>("QVariant");
}

void ThreadRunClient::runClient()
{
    //qDebug()<<"sssssssssss";
    userAccount="4228110010016";
    userPassword="8251032ssq";
    flag=0;
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, SIGNAL(timeout()), this, SLOT(getTimeResetProgress()));
    startRequest( QUrl("http://www.faxuanyun.com/bps/index.html"));
}
void ThreadRunClient::setAllCookies()
{
    QString str;
    str="{\"userAccount\":\""+userAccount+"\"}";
    QByteArray stdByte = str.toUtf8();//QByteArray::fromStdString(str);
    QNetworkCookie cy("loginUser",stdByte) ;
    allcookies.append(cy);

}
QString ThreadRunClient::secretResult(QString input)
{
    //QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    //QString usercount=";"+userAccount;
    //QString rid=";"+terminalCode+";"+sid;
    //QString input=timestamp+usercount+rid+";-1";
    //qDebug() << "input:" << input;
    QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::CBC, QAESEncryption::PKCS7);
    //QByteArray key = QString("5KLwKRd7tjsEQZmM").toUtf8();
    //QByteArray iv = QString("erVtH2aehRE8WrTE").toUtf8();

    QByteArray encodeText = encryption.encode(input.toUtf8(), key.toUtf8(), iv.toUtf8());
    QString encodeStr = encodeText.toBase64();
    //qDebug() << "encodeText:" << encodeText;
    //qDebug() << "encodeStr:" << encodeStr;
    return encodeStr;
}
void ThreadRunClient::imageToHex(QImage &image)
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
//////////////////////////////////////////////////////////////////////////
void ThreadRunClient::startRequest(const QUrl &requestedUrl){
    emit returnLog("login");
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);

    //get方式发送请求
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    //qt6后默认自动重定向，设置为手动才能得到重定向以前的数据
    reply = manager->get(req);
    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyCookies);

}
void ThreadRunClient::replyCookies()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    // <2>检测网页返回状态码，常见是200，404等，200为成功
    int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    //qDebug() << "接收到的数据" <<reply->readAll();
    if (statusCode == 302)
    {
        QVariant variantCookies = reply->header(QNetworkRequest::SetCookieHeader);
        QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie> >(variantCookies);

        cookie=cookies.at(0);

        //QString DataAsString =cookie.toRawForm();//toRawForm方法是转换为QByteArray
        //qDebug() <<variantCookies <<variantCookies.toString() << "   ---- " <<cookies.size()<<" "<<cookies.at(0)<<" "<<DataAsString;
        // 数据读取完成之后，清除reply
        reply->deleteLater();
        reply = nullptr;
        getCodeWithCookies(QUrl("http://www.faxuanyun.com/service/gc.html?timestamp="),cookie);

    }
    else
    {
        emit returnLog("error code=1");
    }
}
//////////////////////////////////////////////////////////////////////
void ThreadRunClient::getCodeWithCookies(const QUrl &requestedUrl,const QNetworkCookie &cookies)
{
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookies);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);

    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyCode);
}
void ThreadRunClient::replyCode()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode <300)
    {
        //qDebug() << "statusCode:" << statusCode;
        //qDebug() << "接收到的数据" <<reply->readAll();
        QByteArray imageData=reply->readAll();
        //qDebug() << "接收到的数据" <<imageData;
        QImage image;
        image=QImage::fromData(imageData);
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
        reply->deleteLater();
        reply = nullptr;
        if(str.length()!=4)
        {
            qDebug()<<"llll"<<str.length();
            //startRequest(QUrl("http://www.faxuanyun.com/bps/index.html"));
            getCodeWithCookies(QUrl("http://www.faxuanyun.com/service/gc.html?timestamp="),cookie);
        }
        else
        {
            //qDebug()<<cookie.value();
            QString strurl="http://www.faxuanyun.com/bss/service/userService!doUserLogin.do?userAccount="+userAccount+"&userPassword="+userPassword+"&code="+str+"&rid="+cookie.value();
            //strurl=strurl.left(strurl.size()-1);
            loginfx(QUrl(strurl));
        }

    }
    else
    {
        emit returnLog("error code=2");
    }
}
/////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::loginfx(const QUrl &requestedUrl)
{
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);
    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyLogin);
}
void ThreadRunClient::replyLogin()
{
    //QVariant var;
    //var.setValue(cookie);
    //req.setHeader(QNetworkRequest::CookieHeader,var);

    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString strjson;
    strjson=reply->readAll();
    // 数据读取完成之后，清除reply
    reply->deleteLater();
    reply = nullptr;
    QJsonParseError jsonError;
    QJsonValue thisValue;
    QJsonDocument doucment = QJsonDocument::fromJson(strjson.toUtf8(), &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject())
        {  // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("data"))
            {
                QJsonValue dataValue = object.value("data");
                if(dataValue.type()== QJsonValue::Object)
                {
                    QJsonObject dataObj = dataValue.toObject();
                    if(dataObj.contains("userName"))
                    {
                        thisValue = dataObj.value("userName");
                        userNameOne=thisValue.toVariant().toString().toUtf8();
                    }
                    if(dataObj.contains("domainCode"))
                    {
                        thisValue = dataObj.value("domainCode");
                        domainCode=thisValue.toVariant().toString();
                    }
                    if(dataObj.contains("sid"))
                    {
                        thisValue = dataObj.value("sid");
                        sid=thisValue.toVariant().toString();
                    }
                    if(dataObj.contains("rankId"))
                    {
                        thisValue = dataObj.value("rankId");
                        rankId=thisValue.toVariant().toString();
                    }
                    if(dataObj.contains("politicsCode"))
                    {
                        thisValue = dataObj.value("politicsCode");
                        politicsCode=thisValue.toVariant().toString();
                    }
                    if(dataObj.contains("areaCode"))
                    {
                        thisValue = dataObj.value("areaCode");
                        areaCode=thisValue.toVariant().toString();
                    }

                }
            }
            if (object.contains("code"))
            {
                QJsonValue nameValue = object.value("code");
                QString code=nameValue.toVariant().toString();

                if(code=="300")
                {
                    //qDebug() << "code = " << code;
                    getCodeWithCookies(QUrl("http://www.faxuanyun.com/service/gc.html?timestamp="),cookie);
                }
                if(code=="200")
                {
                    //qDebug() << "code = " << code;
                    getCommResources(QUrl("http://www.faxuanyun.com/bps/common/comm_resources.js?v=20240411"));

                }
            }
        }
        else emit returnLog("error code=3");
    }
    else emit returnLog("error code=3");
}
/////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getCommResources(const QUrl &requestedUrl)
{
    emit returnLog("receive resource");
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyCommResources);
}
void ThreadRunClient::replyCommResources()
{
    //QVariant var;
    //var.setValue(cookie);
    //req.setHeader(QNetworkRequest::CookieHeader,var);

    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString strjson;
    strjson=reply->readAll();
    // 数据读取完成之后，清除reply
    reply->deleteLater();
    reply = nullptr;
    QRegExp rxtcode("terminalCode:\"[A-Za-z0-9]+\"");
    rxtcode.indexIn(strjson);
    QRegExp rxtcode1("\"[A-Za-z0-9]+\"");
    rxtcode1.indexIn(rxtcode.cap(0));
    terminalCode=rxtcode1.cap(0);
    terminalCode.replace("\"","");
    //qDebug() << terminalCode;

    QRegExp rxkey("key:\"[A-Za-z0-9]+\"");
    rxkey.indexIn(strjson);
    QRegExp rxkey1("\"[A-Za-z0-9]+\"");
    rxkey1.indexIn(rxkey.cap(0));
    key=rxkey1.cap(0);
    key.replace("\"","");
    //qDebug() << key;

    QRegExp rxiv("iv:\"[A-Za-z0-9]+\"");
    rxiv.indexIn(strjson);
    QRegExp rxiv1("\"[A-Za-z0-9]+\"");
    rxiv1.indexIn(rxiv.cap(0));
    iv=rxiv1.cap(0);
    iv.replace("\"","");
    //qDebug() << iv;

    QLocale locale = QLocale::English;
    QString format = "ddd MMMM dd yyyy hh:mm:ss";
    QString currentTime =locale.toString(QDateTime::currentDateTimeUtc(), format) + " GMT+0800";
    QString strUrl="http://www.faxuanyun.com/bss/service/getIndustryCodes?domainCode="+domainCode+"&time="+currentTime;
    getIndustryCode(QUrl(strUrl));
}
/////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getIndustryCode(const QUrl &requestedUrl)
{
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyIndustryCode);
}
void ThreadRunClient::replyIndustryCode()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString str;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    QRegExp rxtcode("\r\n[A-Za-z0-9]+\r\n");
    rxtcode.indexIn(str);
    industryCodes=rxtcode.cap(0);
    industryCodes.replace("\r\n","");
    //qDebug() << industryCodes;

    QLocale locale = QLocale::English;
    QString format = "ddd MMMM dd yyyy hh:mm:ss";
    QString currentTime =locale.toString(QDateTime::currentDateTimeUtc(), format) + " GMT+0800";
    QString strurl="http://www.faxuanyun.com/sss/service/getusercourseinfo?userAccount="+userAccount+"&time="+currentTime;
    getCourseInfo(QUrl(strurl));
}
/////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getCourseInfo(const QUrl &requestedUrl)
{
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyCourseInfo);
}
void ThreadRunClient::replyCourseInfo()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString str;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(str.toUtf8(), &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))  // 解析未发生错误
    {
        if (doucment.isArray())  // JSON 文档为数组
        {
            QJsonArray array = doucment.array();  // 转化为数组
            int nSize = array.size();  // 获取数组大小
            int flag=0;
            for (int i = 0; i < nSize; ++i)   // 遍历数组
            {
                QJsonValue value = array.at(i);
                if(flag==1)
                {
                    flag=0;
                    totalPeriod=value.toString();
                }
                if(value=="totalPeriod")flag=1;
            }
            //getLearn(QUrl("http://www.faxuanyun.com/sss/service/coursewareService!getLearnPeriod.do"));
        }//qDebug()<<totalPeriod;
        else emit returnLog("error code=4.1");
    }
    else emit returnLog("error code=4.2");
    getLearn(QUrl("http://www.faxuanyun.com/sss/service/coursewareService!getLearnPeriod.do"));
}
/////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getLearn(const QUrl &requestedUrl)
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    QString usercount=";"+userAccount;
    QString rid=";"+terminalCode+";"+sid;
    QString input=timestamp+usercount+rid+";-1";

    QString skey=secretResult(input);
    QString ss="userAccount="+userAccount+"&terminalCode="+terminalCode+"&skey="+skey;
    ss.replace("+","%2B");//将+号转换为%2B,否则后端转换位将+号转为空格
    QByteArray dataArray;

    dataArray=ss.toUtf8();
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyLearn);
}
void ThreadRunClient::replyLearn()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString strjson;;
    strjson=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    QRegExp rxtcode("[0-9]+_finished");
    int pos = 0;
    //QStringList list;
    while ((pos = rxtcode.indexIn(strjson, pos)) != -1)
    {
        finishList << rxtcode.cap(0).split("_")[0];
        pos += rxtcode.matchedLength();
    }
    QString tempstr=domainCode;
    QString xianCode=tempstr.replace(11,4,"0000");
    QString shiCode=tempstr.replace(19,6,"000000");
    QString shengCode=tempstr.replace(6,9,"000000000");
    QString guoCode=tempstr.replace(4,11,"00000000000");
    QLocale locale = QLocale::English;
    QString format = "ddd MMMM dd yyyy hh:mm:ss";
    QString currentTime =locale.toString(QDateTime::currentDateTimeUtc(), format) + " GMT+0800";
    QString strurl="http://www.faxuanyun.com/sss/service/getstudycoursebyindusrtycodenew?"
                     "fileds=STUDY:COURSE:STR.12_"+guoCode+"_"+rankId+",STUDY:COURSE:STR.12_"+guoCode+"_"+politicsCode+","
                     "STUDY:COURSE:STR.12_"+shengCode+"_"+rankId+",STUDY:COURSE:STR.12_"+shengCode+"_"+politicsCode+","
                     "STUDY:COURSE:STR.12_"+shiCode+"_"+rankId+",STUDY:COURSE:STR.12_"+shiCode+"_"+politicsCode+","
                     "STUDY:COURSE:STR.12_"+xianCode+"_"+rankId+",STUDY:COURSE:STR.12_"+xianCode+"_"+politicsCode+","
                     "STUDY:COURSE:STR.12_"+domainCode+"_"+rankId+",STUDY:COURSE:STR.12_"+domainCode+"_"+politicsCode+""
                     "&industryCodes="+industryCodes+"&languageType=4&time="+currentTime;
    //qDebug()<<strurl;
    getMustList(QUrl(strurl));

}
/////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getMustList(const QUrl &requestedUrl)
{
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyMustList);
}
void ThreadRunClient::replyMustList()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString str;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;

    QString strva;
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(str.toUtf8(), &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))  // 解析未发生错误
    {
        if (doucment.isArray())  // JSON 文档为数组
        {
            QJsonArray array = doucment.array();  // 转化为数组
            QJsonValue value = array.at(0);
            strva =value.toObject().value("3").toString();
        }
        else emit returnLog("error code=5.1");
    }
    else emit returnLog("error code=5.2");
    doucment = QJsonDocument::fromJson(strva.toUtf8(), &jsonError);  // 转化为 JSON 文档
    //QVector<QVector<QString>> carray;
    QVector<QString> temp;
    //tableArray.clear();
    for(int i=0;i<tableArray.size();i++)
    {
        tableArray[i].clear();
    }
    tableArray.clear();
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))  // 解析未发生错误
    {
        if (doucment.isArray())  // JSON 文档为数组
        {
            QJsonArray array = doucment.array();  // 转化为数组
            int nSize = array.size();  // 获取数组大小
            for (int i = 0; i < nSize; ++i)   // 遍历数组
            {
                QJsonValue value = array.at(i);
                //qDebug()<<value.toObject().value("courseId").toDouble();//.toString();
                //qDebug()<<value.toObject().value("courseName").toString();
                QString cid;
                cid=QString::number(value.toObject().value("courseId").toDouble());
                temp.append(QString::number(value.toObject().value("courseId").toDouble()));
                temp.append(value.toObject().value("courseName").toString());
                //QString ss=value.toObject().value("courseWarePeriod").toString();
                if(finishList.indexOf(cid)>=0)
                {
                    temp.append("yes");
                }
                else if(value.toObject().value("courseWarePeriod").toString()=="0")
                {
                    temp.append("wait");
                }
                else
                {
                    temp.append("no");
                }
                tableArray.append(temp);
                temp.clear();
            }
            //tableArray.resize(tableArray.size());
            //QVariant dataVar;
            //dataVar.setValue(tableArray);

            //emit returnResult(dataVar,totalPeriod);
        }
        else emit returnLog("error code=5.3");
    }
    else emit returnLog("error code=5.4");
    QString tempstr=domainCode;
    QString xianCode=tempstr.replace(11,4,"0000");
    QString shiCode=tempstr.replace(19,6,"000000");
    QString shengCode=tempstr.replace(6,9,"000000000");
    QString guoCode=tempstr.replace(4,11,"00000000000");
    QLocale locale = QLocale::English;
    QString format = "ddd MMMM dd yyyy hh:mm:ss";
    QString currentTime =locale.toString(QDateTime::currentDateTimeUtc(), format) + " GMT+0800";
    QString strurl="http://www.faxuanyun.com/sss/service/getstudycoursebyindusrtycodenew?"
                     "fileds=STUDY:COURSE:STR.12_"+guoCode+"_"+rankId+",STUDY:COURSE:STR.11_"+guoCode+"_"+politicsCode+","
                     "STUDY:COURSE:STR.11_"+shengCode+"_"+rankId+",STUDY:COURSE:STR.11_"+shengCode+"_"+politicsCode+","
                     "STUDY:COURSE:STR.11_"+shiCode+"_"+rankId+",STUDY:COURSE:STR.11_"+shiCode+"_"+politicsCode+","
                     "STUDY:COURSE:STR.11_"+xianCode+"_"+rankId+",STUDY:COURSE:STR.11_"+xianCode+"_"+politicsCode+","
                     "STUDY:COURSE:STR.11_"+domainCode+"_"+rankId+",STUDY:COURSE:STR.11_"+domainCode+"_"+politicsCode+""
                     "&industryCodes="+industryCodes+"&languageType=4&time="+currentTime;
    getElectiveList(QUrl(strurl));
    //getProgress(QUrl("http://www.faxuanyun.com/sss/service/coursewareService!getLearnPeriod.do"));
}
////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getElectiveList(const QUrl &requestedUrl)
{
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyElectiveList);
}
void ThreadRunClient::replyElectiveList()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString str;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;

    QString strva;
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(str.toUtf8(), &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))  // 解析未发生错误
    {
        if (doucment.isArray())  // JSON 文档为数组
        {
            QJsonArray array = doucment.array();  // 转化为数组
            QJsonValue value = array.at(0);
            strva =value.toObject().value("3").toString();
        }
        else emit returnLog("error code=6.1");
    }
    else emit returnLog("error code=6.2");
    doucment = QJsonDocument::fromJson(strva.toUtf8(), &jsonError);  // 转化为 JSON 文档
    //QVector<QVector<QString>> carray;
    QVector<QString> temp;
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))  // 解析未发生错误
    {
        if (doucment.isArray())  // JSON 文档为数组
        {
            QJsonArray array = doucment.array();  // 转化为数组
            int nSize = array.size();  // 获取数组大小
            for (int i = 0; i < nSize; ++i)   // 遍历数组
            {
                QJsonValue value = array.at(i);
                //qDebug()<<value.toObject().value("courseId").toDouble();//.toString();
                //qDebug()<<value.toObject().value("courseName").toString();
                QString cid;
                cid=QString::number(value.toObject().value("courseId").toDouble());
                temp.append(QString::number(value.toObject().value("courseId").toDouble()));
                temp.append(value.toObject().value("courseName").toString());
                if(finishList.indexOf(cid)>=0)
                {
                    temp.append("yes");
                }
                else if(value.toObject().value("courseWarePeriod").toString()=="0")
                {
                    temp.append("wait");
                }
                else
                {
                    temp.append("no");
                }
                tableArray.append(temp);
                temp.clear();
            }
            tableArray.resize(tableArray.size()-1);//去掉最后1行
            QVariant dataVar;
            dataVar.setValue(tableArray);

            emit returnResult(dataVar,totalPeriod+"  "+userAccount);
        }
        else emit returnLog("error code=6.3");
    }
    else emit returnLog("error code=6.4");
    getProgress(QUrl("http://www.faxuanyun.com/sss/service/coursewareService!getLearnPeriod.do"));
}
/////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getProgress(const QUrl &requestedUrl)
{
    id="";
    int flag=0;
    for(QVector<QVector<QString>>::iterator pRow = tableArray.begin();pRow!=tableArray.end();pRow++)
    {
        if(flag==1)break;

        for(QVector<QString>::iterator pColoum = pRow->begin();pColoum!=pRow->end();pColoum++)
        {
            if(*pColoum=="no")
            {
                pColoum=pColoum-2;
                id=*pColoum;
                flag=1;
                break;
            }
        }
    }
    if(id=="")
    {
        emit returnLog("study end");
        return;
    }

    qDebug()<<id;
    //emit returnLog(id);

    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    QString usercount=";"+userAccount;
    QString rid=";"+terminalCode+";"+sid;
    QString input=timestamp+usercount+rid+";"+id;

    QString skey=secretResult(input);
    QString ss="userAccount="+userAccount+"&terminalCode="+terminalCode+"&skey="+skey;
    ss.replace("+","%2B");//将+号转换为%2B,否则后端转换位将+号转为空格
    QByteArray dataArray;

    dataArray=ss.toUtf8();
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyProgress);
}
void ThreadRunClient::replyProgress()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString str;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;

    QLocale locale = QLocale::English;
    QString format = "ddd MMMM dd yyyy hh:mm:ss";
    QString currentTime =locale.toString(QDateTime::currentDateTimeUtc(), format) + " GMT+0800";
    QString strurl;
    strurl="http://www.faxuanyun.com/sss/service/getcourseware?courseId="+id+"&time="+currentTime;
    getCoursewareId(QUrl(strurl));
}
/////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getCoursewareId(const QUrl &requestedUrl)
{
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replayCoursewareId);

}
void ThreadRunClient::replayCoursewareId()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString str;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;

    QRegExp rxtcode("[0-9]+_");
    rxtcode.indexIn(str);
    coursewareId=rxtcode.cap(0);
    coursewareId.replace("_","");
    //emit returnLog(coursewareId);
    qDebug()<<coursewareId;
    getBeginLearn(QUrl("http://www.faxuanyun.com/sss/service/coursewareService!doBeginLeaningCheck.do"));
}
///////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getBeginLearn(const QUrl &requestedUrl)
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    QString usercount=";"+userAccount;
    QString rid=";"+sid;
    QString input=timestamp+usercount+rid+";"+id+";"+coursewareId+";"+terminalCode;

    QString skey=secretResult(input);
    QString ss="userAccount="+userAccount+"&terminalCode="+terminalCode+"&skey="+skey+"&time="+timestamp;
    ss.replace("+","%2B");//将+号转换为%2B,否则后端转换位将+号转为空格
    QByteArray dataArray;

    dataArray=ss.toUtf8();
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyBeginLearn);
}
void ThreadRunClient::replyBeginLearn()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString str;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    QLocale locale = QLocale::English;
    QString format = "ddd MMMM dd yyyy hh:mm:ss";
    QString currentTime =locale.toString(QDateTime::currentDateTimeUtc(), format) + " GMT+0800";
    QString strurl="http://www.faxuanyun.com/sss/service/getcoursecontent?coursewareId="+coursewareId+"&time="+currentTime;
    getCourseContent(QUrl(strurl));
}
///////////////////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getCourseContent(const QUrl &requestedUrl)
{
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyCourseContent);
}
void ThreadRunClient::replyCourseContent()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString str;
    str=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    QString strjson=str.mid(str.indexOf("{"));
    QJsonParseError jsonError;
    QJsonValue thisValue;
    QJsonDocument doucment = QJsonDocument::fromJson(strjson.toUtf8(), &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject())
        {  // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("courseDuration"))
            {
                QJsonValue dataValue = object.value("courseDuration");
                courseTime=dataValue.toVariant().toString();
                qDebug()<<courseTime;
                //emit returnLog(courseTime);
            }
            else emit returnLog("error code=7.1");
        }
        else emit returnLog("error code=7.2");
    }
    else
    {
        emit returnLog("error code=7.3");

    }
    getLearnProgress(QUrl("http://www.faxuanyun.com/sss/service/coursewareService!doGetLearnProgress.do"));
}
////////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getLearnProgress(const QUrl &requestedUrl)
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());

    QString input=userAccount+";"+sid+";"+id+";"+coursewareId+";"+timestamp+";"+terminalCode;

    QString skey=secretResult(input);
    QString ss="userAccount="+userAccount+"&terminalCode="+terminalCode+"&skey="+skey;
    ss.replace("+","%2B");//将+号转换为%2B,否则后端转换位将+号转为空格
    QByteArray dataArray;

    dataArray=ss.toUtf8();
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyLearnProgress);
}
void ThreadRunClient::replyLearnProgress()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString strjson;
    strjson=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    QJsonParseError jsonError;
    QJsonValue thisValue;
    QJsonDocument doucment = QJsonDocument::fromJson(strjson.toUtf8(), &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject())
        {  // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("data"))
            {
                QJsonValue dataValue = object.value("data");
                learnTime=dataValue.toVariant().toString();
                qDebug()<<learnTime;
            }
            else emit returnLog("error code=8.1");
        }
        else emit returnLog("error code=8.2");
    }
    else emit returnLog("error code=8.3");
    //显示当前课程数据
    QString temp=id+" "+coursewareId+" "+courseTime+" "+learnTime;
    emit returnLog(temp);
    //getResetProgress(QUrl("http://www.faxuanyun.com/sss/service/coursewareService!doResetLearnProgress.do"));
    getInitStudy(QUrl("http://www.faxuanyun.com/sss/service/studyExamService!doInitStudyExam.do"));
}
//////////////////////////////////////////////////////
void ThreadRunClient::getInitStudy(const QUrl &requestedUrl)
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());

    QString input=timestamp+";"+userAccount+";"+sid+";"+id+"_"+coursewareId+";"+terminalCode;

    QString skey=secretResult(input);
    QString ss="userAccount="+userAccount+"&terminalCode="+terminalCode+"&type=2&secondaryKey="+skey+"&time="+timestamp;
    ss.replace("+","%2B");//将+号转换为%2B,否则后端转换位将+号转为空格
    QByteArray dataArray;

    dataArray=ss.toUtf8();
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyInitStudy);
}
void ThreadRunClient::replyInitStudy()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString strjson;
    strjson=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    getResetProgress(QUrl("http://www.faxuanyun.com/sss/service/coursewareService!doResetLearnProgress.do"));
}

/////////////////////////////////////////////////////////////
void ThreadRunClient::getResetProgress(const QUrl &requestedUrl)
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());

    QString input=timestamp+";"+sid+";"+userAccount+";"+terminalCode+";"+id+";"+coursewareId+";"+learnTime;

    QString lkey=secretResult(input);

    QString input2=timestamp+";"+sid+";"+userAccount+";-1;0";
    QString temp=secretResult(input2);
    secretKey=secretResult(temp);
    int num=0;
    if(courseTime.toInt()-learnTime.toInt()>0)
    {num=0;}
    else
    {num=1;}

    QString finish = QString::number(num);
    QString ss="userAccount="+userAccount+"&terminalCode="+terminalCode+"&secretKey="+secretKey+"&learnKey="+lkey+"&isFinished="+finish+"&source=1";
    ss.replace("+","%2B");//将+号转换为%2B,否则后端转换位将+号转为空格
    QByteArray dataArray;

    dataArray=ss.toUtf8();
    url = requestedUrl;
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyResetProgress);
}
void ThreadRunClient::replyResetProgress()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString strjson;
    strjson=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    timer->start(180000);//定时3分钟
    emit returnLog("start qtime");

}
/////////////////////////////////////////////////////////////////////////
void ThreadRunClient::getTimeResetProgress()
{

    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    int lt=learnTime.toInt();
    lt=lt+180;
    learnTime=QString::number(lt);
    QString input=timestamp+";"+sid+";"+userAccount+";"+terminalCode+";"+id+";"+coursewareId+";"+learnTime;
    QString lkey=secretResult(input);
    flag=0;
    if(courseTime.toInt()-learnTime.toInt()>0)
    {flag=0;}
    else
    {
        flag=1;
        timer->stop();
        emit returnLog("stop qtime");
    }

    //emit returnLog("qtime 3min");
    QString temp=id+" "+coursewareId+" "+courseTime+" "+learnTime;
    emit returnLog(temp);

    QString finish = QString::number(flag);
    QString ss="userAccount="+userAccount+"&terminalCode="+terminalCode+"&secretKey="+secretKey+"&learnKey="+lkey+"&isFinished="+finish+"&source=1";
    ss.replace("+","%2B");//将+号转换为%2B,否则后端转换位将+号转为空格
    QByteArray dataArray;

    dataArray=ss.toUtf8();
    url = QUrl("http://www.faxuanyun.com/sss/service/coursewareService!doResetLearnProgress.do");
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyTimeResetProgress);
}
void ThreadRunClient::replyTimeResetProgress()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString strjson;
    strjson=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    if(flag==1)getEndStudy();
}
/////////////////////////////////////////////////////////
void ThreadRunClient::getEndStudy()
{
    emit returnLog("cur lesson end,next");
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    int lt=learnTime.toInt();
    lt=lt+180;
    learnTime=QString::number(lt);
    QString input=timestamp+"|"+userAccount+"|"+terminalCode+"|"+domainCode+"|"+sid+"|"+id+"|"+coursewareId+"|1|";
    QString lkey=secretResult(input);
    QString input1=coursewareId+";1";
    QString rkey=secretResult(input1);
    QString ss="userAccount="+userAccount+"&terminalCode="+terminalCode+"&secretKey="+secretKey+"&secondaryKey="+lkey+"&rkey="+rkey+"&timestamp="+timestamp;
    ss.replace("+","%2B");//将+号转换为%2B,否则后端转换位将+号转为空格
    QByteArray dataArray;

    dataArray=ss.toUtf8();
    url = QUrl("http://www.faxuanyun.com/sss/service/coursewareService!endStudy.do");
    manager = new QNetworkAccessManager(this);
    req.setUrl(url);
    QVariant var;
    var.setValue(cookie);
    req.setHeader(QNetworkRequest::CookieHeader,var);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded;chatset=UTF-8"));
    reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&ThreadRunClient::replyEndStudy);
}
void ThreadRunClient::replyEndStudy()
{
    if (reply->error()){
        //qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    //int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "statusCode:" << statusCode;
    QString strjson;
    strjson=reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    getCodeWithCookies(QUrl("http://www.faxuanyun.com/service/gc.html?timestamp="),cookie);
    //startRequest( QUrl("http://www.faxuanyun.com/bps/index.html"));
}
