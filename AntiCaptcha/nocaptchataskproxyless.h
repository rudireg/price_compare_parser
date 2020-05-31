#ifndef NOCAPTCHATASKPROXYLESS_H
#define NOCAPTCHATASKPROXYLESS_H

#include "anticaptcha.h"

class NoCaptchaTaskProxyless : public AntiCaptcha
{
    Q_OBJECT
public:
    explicit NoCaptchaTaskProxyless(QObject *parent);
protected:
    QString createParams(QString siteKey, QString url);
};

#endif // NOCAPTCHATASKPROXYLESS_H
