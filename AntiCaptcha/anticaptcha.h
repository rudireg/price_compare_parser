#ifndef ANTICAPTCHA_H
#define ANTICAPTCHA_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "Network/rhttp.h"

class AntiCaptcha : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString _taskId READ taskId WRITE setTaskId)
    Q_PROPERTY(int _errorId READ errorId WRITE setErrorId)
    Q_PROPERTY(QString _errorCode READ errorCode WRITE setErrorCode)
    Q_PROPERTY(QString _errorDescription READ errorDescription WRITE setErrorDescription)
public:
    explicit AntiCaptcha(QObject *parent = nullptr);
    QString resolve(QString siteKey, QString url);
    QString taskId() {return this->_taskId;}
    void setTaskId(QString val) {this->_taskId = val;}
    QString errorCode() {return this->_errorCode;}
    void setErrorCode(QString val) {this->_errorCode = val;}
    QString errorDescription() {return this->_errorDescription;}
    void setErrorDescription(QString val) {this->_errorDescription = val;}
    int errorId() {return this->_errorId;}
    void setErrorId(int val) {this->_errorId = val;}
    QString humanError(QString errorCode);
private:

signals:

protected:
    RHttp *http;
    QString _taskId;
    int _errorId;
    QString _errorCode;
    QString _errorDescription;
    QString getTaskResult(QString taskId);
    QString createTask(QString siteKey, QString url);
    virtual QString createParams(QString siteKey, QString url) = 0;
};

#endif // ANTICAPTCHA_H
