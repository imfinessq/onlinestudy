#ifndef THREADONLINE_H
#define THREADONLINE_H

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
//#include "qaesencryption.h"

class ThreadOnline : public QObject
{
    Q_OBJECT
public:
    explicit ThreadOnline(QObject *parent = nullptr);
    void imageToHex(QImage &image);
    void runClientOnline();
    void getCode();
    void replyCode();
    void loginOnline(QString code);
    void replyLogin();
    void getStudent();
    void replyStudent();
    void getLessonList();
    void replyLessonList();

    void getPlayId();
    void replyPlayId();

    void replyTimeResetProgress();
private:
    QString userAccount;
    QString userPassword;
    QTimer *timer;
    QUrl url;
    QNetworkRequest req;
    QNetworkReply *reply;
    QNetworkAccessManager *manager;
    QNetworkCookie  cookie;
    QList<QNetworkCookie> cookies;
    QString lablezx;
    QString uuid;
    QString ucid;
    QString lessonLocation;
    QString playId;
    QString lessonTimeSecond;
private slots:
    void getTimeResetProgress();
signals:
    void returnResult(QVariant,QString);
    void returnLog(QString text);
};

#endif // THREADONLINE_H
