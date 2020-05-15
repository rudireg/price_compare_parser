#include "mactacmru.h"

Mactacmru::Mactacmru(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://mactak-m.ru/";
}

/**
 * @brief Mactacmru::search
 * @param article
 * @return
 */
QString Mactacmru::search(Article &article)
{
    article.sites[this->siteIndex].searchUrl =  QString("https://mactak-m.ru/search/?q=%1").arg(article.article);
    QString params = QString("API_SEARCH_TITLE_AJAX=Y&q=%1").arg(article.article);
    if (this->http->post(this->siteUrl, params.toUtf8())) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief Mactacmru::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Mactacmru::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Mactacmru::parsePrice
 * @param block
 * @return
 */
float Mactacmru::parsePrice(RString block)
{
    QString boudoir = "<span class=\"api-item-price\">";
    RString priceStr = block.cut_str_to_str(boudoir, "</span>");
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
 * @brief Mactacmru::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Mactacmru::parseProductUrl(RString block, QString domain)
{
    QString url = block.cut_str_to_str("<a href=\"", "\"");
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
 * @brief Mactacmru::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Mactacmru::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString boudoir = "<div class=\"api-item\">";
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
 * @brief Mactacmru::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Mactacmru::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<span class=\"api-item-name\">", "<span class=\"api-item-prices\"");
        if (title.contains(article)) {
            return block;
        }
    }
    return "";
}
