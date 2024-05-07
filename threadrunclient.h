#ifndef THREADRUNCLIENT_H
#define THREADRUNCLIENT_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QImage>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QMetaType>
#include <QVariant>
#include "qaesencryption.h"

Q_DECLARE_METATYPE(QVector<QVector<QString> >);
class ThreadRunClient:public QObject
{
    Q_OBJECT
public:
    explicit ThreadRunClient(QObject *parent = nullptr);
    void startRequest(const QUrl &requestedUrl);
    void replyCookies();
    void getCodeWithCookies(const QUrl &requestedUrl,const QNetworkCookie &cookies);
    void replyCode();
    void imageToHex(QImage &image);
    void loginfx(const QUrl &requestedUrl);
    void replyLogin();
    void getCommResources(const QUrl &requestedUrl);
    void replyCommResources();
    //void getSid(const QUrl &requestedUrl);
    //void replySid();
    void getMustList(const QUrl &requestedUrl);
    void replyMustList();

    void getElectiveList(const QUrl &requestedUrl);
    void replyElectiveList();

    void getIndustryCode(const QUrl &requestedUrl);
    void replyIndustryCode();
    //void getDomainCode(const QUrl &requestedUrl);
    //void replyDomainCode();
    QString secretResult(QString);
    void getCourseInfo(const QUrl &requestedUrl);
    void replyCourseInfo();
    void getLearn(const QUrl &requestedUrl);
    void replyLearn();
    void getProgress(const QUrl &requestedUrl);
    void replyProgress();
    void getCoursewareId(const QUrl &requestedUrl);
    void replayCoursewareId();
    void getBeginLearn(const QUrl &requestedUrl);
    void replyBeginLearn();
    void getCourseContent(const QUrl &requestedUrl);
    void replyCourseContent();
    void getLearnProgress(const QUrl &requestedUrl);
    void replyLearnProgress();

    void getInitStudy(const QUrl &requestedUrl);
    void replyInitStudy();

    void getResetProgress(const QUrl &requestedUrl);
    void replyResetProgress();

    //void getTimeResetProgress();
    void replyTimeResetProgress();

    void getEndStudy();
    void replyEndStudy();

    void setAllCookies();

    void runClient();//线程要执行的槽函数
private:
    QUrl url;
    QNetworkRequest req;
    QNetworkReply *reply;
    QNetworkAccessManager *manager;
    QNetworkCookie  cookie;
    QString key;
    QString iv;
    QString terminalCode;
    QString userAccount;
    QString userPassword;
    QString sid;
    QString industryCodes;
    QString domainCode;
    QString userNameOne;
    QList<QNetworkCookie> allcookies;
    QString totalPeriod;
    QStringList finishList;
    QVector<QVector<QString>> tableArray;
    QString id;
    QString coursewareId;
    QString courseTime;
    QString learnTime;
    QTimer *timer;
    QString secretKey;
    QString rankId;
    QString politicsCode;
    QString areaCode;

    int flag;
private slots:
    void getTimeResetProgress();
signals:
    //void message(const QString& info);//用信号传递相关信息给主线程那边
    void returnResult(QVariant,QString);
    void returnLog(QString text);
};

#endif // THREADRUNCLIENT_H
