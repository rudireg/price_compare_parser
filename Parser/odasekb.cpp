#include "odasekb.h"

/**
 * @brief OdasEkb::OdasEkb
 * @param parent
 */
OdasEkb::OdasEkb(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://odas-ekb.ru/";
    this->_productUrlDetails = "";
}

/**
 * @brief OdasEkb::search
 * @param article
 * @return
 */
QString OdasEkb::search(Article &article)
{
    QString url = QString("https://odas-ekb.ru/live-search?q=%1").arg(article.article.trimmed());
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief OdasEkb::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString OdasEkb::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief OdasEkb::parsePrice
 * @param block
 * @return
 */
float OdasEkb::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<span class=\"product-controls-current-price", "</span>");
    if (priceStr.isEmpty()) return 0;
    if (priceStr.contains(",")) {
        priceStr = priceStr.cut_to_str(",");
    }
    QRegExp rx("(\\D+)");
    priceStr.replace(rx, "");
    if (priceStr.isEmpty()) return 0;
    return priceStr.toFloat();
}

/**
 * @brief OdasEkb::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString OdasEkb::parseProductUrl(RString block, QString domain)
{
    return this->_productUrlDetails;
}

/**
 * @brief OdasEkb::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> OdasEkb::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString boudoir = "\"url\":";
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
 * @brief OdasEkb::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString OdasEkb::findArticleInBlock(const QList<QString> blocks, QString article)
{
    QString url = "";
    foreach(QString block, blocks) {
        RString tmp = block;
        if (tmp.contains(article)) {
            url = tmp.cut_str_to_str("url\":\"", "\"");
            if (!url.isEmpty()) {
                url.replace("\\/", "/");
                url = "https://odas-ekb.ru" + url;
                if (this->http->get(url)) {
                    this->_productUrlDetails = url;
                    return this->http->inbuffQString();
                }
            }
        }
    }

    return "";
}
