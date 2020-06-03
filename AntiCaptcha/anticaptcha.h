#ifndef ANTICAPTCHA_H
#define ANTICAPTCHA_H

#include <QObject>
#include <QThread>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "Network/rhttp.h"

class AntiCaptcha : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int _taskId READ taskId WRITE setTaskId)
    Q_PROPERTY(int _errorId READ errorId WRITE setErrorId)
    Q_PROPERTY(QString _errorCode READ errorCode WRITE setErrorCode)
    Q_PROPERTY(QString _errorDescription READ errorDescription WRITE setErrorDescription)
    Q_PROPERTY(QString _clientKey READ clientKey WRITE setClientKey)
    Q_PROPERTY(QString _gRecaptchaResponse READ gRecaptchaResponse WRITE setGRecaptchaResponse)
    Q_PROPERTY(QString _status READ status WRITE setStatus)
    Q_PROPERTY(QString _url READ url WRITE setUrl)
    Q_PROPERTY(double _cost READ cost WRITE setCost)
    Q_PROPERTY(QString _ip READ ip WRITE setIp)
    Q_PROPERTY(int _createTime READ createTime WRITE setCreateTime)
    Q_PROPERTY(int _endTime READ endTime WRITE setEndTime)
    Q_PROPERTY(int _solveCount READ solveCount WRITE setSolveCount)
public:
    explicit AntiCaptcha(QObject *parent = nullptr);
    QString resolve(QString siteKey, QString url);
    int taskId() {return this->_taskId;}
    void setTaskId(int val) {this->_taskId = val;}
    QString errorCode() {return this->_errorCode;}
    void setErrorCode(QString val) {this->_errorCode = val;}
    QString errorDescription() {return this->_errorDescription;}
    void setErrorDescription(QString val) {this->_errorDescription = val;}
    int errorId() {return this->_errorId;}
    void setErrorId(int val) {this->_errorId = val;}
    QString clientKey() {return this->_clientKey;}
    void setClientKey(QString val) {this->_clientKey = val;}
    QString gRecaptchaResponse() {return this->_gRecaptchaResponse;}
    void setGRecaptchaResponse(QString val) {this->_gRecaptchaResponse = val;}
    QString status() {return this->_status;}
    void setStatus(QString val) {this->_status = val;}
    QString url() {return this->_url;}
    void setUrl(QString val) {this->_url = val;}
    double cost() {return this->_cost;}
    void setCost(double val) {this->_cost = val;}
    QString ip() {return this->_ip;}
    void setIp(QString val) {this->_ip = val;}
    int createTime() {return this->_createTime;}
    void setCreateTime(int val) {this->_createTime = val;}
    int endTime() {return this->_endTime;}
    void setEndTime(int val) {this->_endTime = val;}
    int solveCount() {return this->_solveCount;}
    void setSolveCount(int val) {this->_solveCount = val;}

    QString humanError(QString errorCode);
private:

signals:

protected:
    RHttp *http;
    QString _clientKey;
    int _taskId;
    int _errorId;
    QString _status;
    QString _url;
    double _cost;
    QString _ip;
    int _createTime;
    int _endTime;
    int _solveCount;
    QString _gRecaptchaResponse;
    QString _errorCode;
    QString _errorDescription;
    QString getTaskResult(int taskId);
    int createTask(QString siteKey, QString url);
    virtual QString createParams(QString siteKey, QString url) = 0;
};

#endif // ANTICAPTCHA_H
