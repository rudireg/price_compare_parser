#include "imperiyaavto43.h"

/**
 * @brief Imperiyaavto43::Imperiyaavto43
 * @param parent
 */
Imperiyaavto43::Imperiyaavto43(QObject *parent, QString captchaclientKey) : Parser(parent), captchaclientKey(captchaclientKey)
{
    this->siteUrl = "https://imperiyaavto43.ru/";
    this->captcha = new NoCaptchaTaskProxyless(this);
    this->captcha->setClientKey(this->captchaclientKey);
    this->gRecaptchaResponse = "";
}

/**
 * @brief Imperiyaavto43::search
 * @param article
 * @return
 */
QString Imperiyaavto43::search(Article &article)
{
    QString redirect;
    QString url = QString("https://imperiyaavto43.ru/site_search?search_term=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    reSearch:
    if (this->http->get(url)) {
        if (302 == this->http->httpStatusCode()) { // Если каптча
            QString location302 = this->http->Location302();
            this->http->get(this->http->Location302());
            RString inb = this->http->inbuffQString();
            QString urlHash = inb.cut_str_to_str("<input type=\"hidden\" value=\"", "\"");
            if (this->gRecaptchaResponse.isEmpty()) {
                emit addTableStatus(this->objectName().toInt(), article.article, QString("%1 Разгадываем каптчу").arg(this->siteUrl), "green");
                emit addLog(QString("%1 Разгадываем каптчу").arg(this->siteUrl));
                reSolveCaptcha:
                this->gRecaptchaResponse = this->captcha->resolve("6LdbfxIUAAAAAN6WYJWiCT4Vd65-0lP1tTOUKZ48", "https://tiu.ru/");
                if (this->gRecaptchaResponse.isEmpty()) {
                    emit addTableStatus(this->objectName().toInt(), article.article, QString("%1 ПОВТОРНО Разгадываем каптчу. ErrorCode: %2").arg(this->siteUrl).arg(this->captcha->errorCode()), "orange");
                    emit addLog(QString("%1 ПОВТОРНО Разгадываем каптчу. errorCode: %2").arg(this->siteUrl).arg(this->captcha->errorCode()));
                    QThread::sleep(5);
                    goto reSolveCaptcha;
                }
            }
            QString param = QString("url=%1&g-recaptcha-response=%2").arg(urlHash).arg(this->gRecaptchaResponse);
            this->http->setReferer(QString("https://tiu.ru/captcha?url=%1").arg(urlHash));
            this->http->post("https://imperiyaavto43.ru/check_captcha", param.toUtf8());
            if (302 == this->http->httpStatusCode()) {
                redirect = this->http->Location302();
                if (redirect.contains("imperiyaavto43.ru")) {
                    emit addTableStatus(this->objectName().toInt(), article.article, QString("%1 Каптча успешно разгадана").arg(this->siteUrl), "green");
                    emit addLog(QString("%1 Каптча успешно разгадана").arg(this->siteUrl));
                    redirect.replace("http://imperiyaavto43.ru", "https://imperiyaavto43.ru");
                    this->http->get(redirect);
                    RString ttt = this->http->inbuffQString();
                    return this->http->inbuffQString();
                } else {
                    this->gRecaptchaResponse.clear();
                    emit addTableStatus(this->objectName().toInt(), article.article, QString("%1 Каптча разгадана не верно").arg(this->siteUrl), "green");
                    emit addLog(QString("%1 Каптча разгадана не верно").arg(this->siteUrl));
                    goto reSearch;
                }
            }
        } else {
            return this->http->inbuffQString();
        }
    }

    return "";
}

/**
 * @brief Imperiyaavto43::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Imperiyaavto43::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Imperiyaavto43::parsePrice
 * @param block
 * @return
 */
float Imperiyaavto43::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("\"price\":\"", "\",\"");
    if (priceStr.isEmpty()) return 0;
    if (priceStr.contains(",")) {
        priceStr = priceStr.cut_to_str(",");
    }
    QRegExp rx("(\\D+)");
    priceStr.replace(rx, "");
    if (priceStr.isEmpty()) return 0;

    RString priceStrDiscount = block.cut_str_to_str("\"discountedPrice\":\"", "\",\"");
    if (priceStrDiscount.isEmpty()) {
        priceStrDiscount = "0";
    } else {
        priceStrDiscount.replace(rx, "");
    }
    if (priceStrDiscount.toInt() > 0 && priceStrDiscount.toInt() < priceStr.toInt()) {
        priceStr = priceStrDiscount;
    }

    return priceStr.toFloat();
}

/**
 * @brief Imperiyaavto43::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Imperiyaavto43::parseProductUrl(RString block, QString domain)
{
    QString url = block.cut_str_to_str("\"url\":\"", "\",\"");
    if (url.isEmpty()) return "";
    if (!url.startsWith("http")) {
        if (domain.endsWith("/")) {
            url.remove(0, 1);
        }
        url.prepend(domain);
    }
    return url;
}

/**
 * @brief Imperiyaavto43::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Imperiyaavto43::splitBlocks(RString inbuf)
{
    inbuf.replace("&#34;", "\"");
    QList<QString> blocks;
    QString startBlock = "__typename\":\"ProductList";
    QString endBlock = "noscript id='init-data'";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = ",\"Product:";
    while (inbuf.contains(boudoir)) {
        inbuf.erase_to(boudoir);
        inbuf.erase_data(1);
        QString block;
        if (inbuf.contains(boudoir)) {
            block = inbuf.cut_to_str(boudoir);
        } else {
            block = inbuf;
        }
        if (!block.isEmpty()) {
            blocks.append(block);
        }
    }
    return blocks;
}

/**
 * @brief Imperiyaavto43::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Imperiyaavto43::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str(",\"name\":\"", "\",\"");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("\"sku\":\"", "\",\"");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
