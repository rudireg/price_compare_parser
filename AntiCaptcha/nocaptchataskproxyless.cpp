#include "nocaptchataskproxyless.h"

NoCaptchaTaskProxyless::NoCaptchaTaskProxyless(QObject *parent) : AntiCaptcha(parent)
{

}

/**
 * @brief NoCaptchaTaskProxyless::createParams
 * @param siteKey
 * @param url
 * @return
 */
QString NoCaptchaTaskProxyless::createParams(QString siteKey, QString url)
{
    QJsonObject recordObject;
    recordObject.insert("clientKey", QJsonValue::fromVariant(this->clientKey()));
    QJsonObject taskObject;
    taskObject.insert("type", "NoCaptchaTaskProxyless");
    taskObject.insert("websiteURL", url);
    taskObject.insert("websiteKey", siteKey);
    recordObject.insert("task", taskObject);
    QJsonDocument doc(recordObject);
    return doc.toJson(QJsonDocument::Compact);
}
