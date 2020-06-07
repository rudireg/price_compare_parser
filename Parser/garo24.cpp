#include "garo24.h"

/**
 * @brief Garo24::Garo24
 * @param parent
 */
Garo24::Garo24(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://garo24.ru/";
}

/**
 * @brief Garo24::search
 * @param article
 * @return
 */
QString Garo24::search(Article &article)
{
    QString url = QString("https://garo24.ru/search/index.php?q=%1&s=").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief Garo24::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Garo24::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Garo24::parsePrice
 * @param block
 * @return
 */
float Garo24::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<span class=\"price_value\">", "</span>");
    if (priceStr.contains(".")) {
        priceStr = priceStr.cut_to_str(".");
    }
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
 * @brief Garo24::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Garo24::parseProductUrl(RString block, QString domain)
{
    QString url = block.cut_str_to_str("<meta itemprop=\"url\" content=\"", "\"/>");
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
 * @brief Garo24::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Garo24::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"search-page";
    QString endBlock = "<b>Отсортировано по релевантности</b>";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"search-preview\">";
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
 * @brief Garo24::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Garo24::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<a class=\"search-title\"", "</a>");
        if (title.contains(article)) {
            return this->gotoProductPage(block);
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<p>", "</p>");
        if (title.contains(article)) {
            return this->gotoProductPage(block);
        }
    }

    return "";
}

/**
 * @brief Garo24::gotoProductPage
 * @param block
 * @return
 */
QString Garo24::gotoProductPage(RString block)
{
    block = block.cut_str_to_str("<a class=\"search-title\"", "</a>");
    if (!block.isEmpty()) {
        QString url = "https://garo24.ru" + block.cut_str_to_str("href=\"", "\"");
        if (this->http->get(url)) {
            return this->http->inbuffQString();
        }
    }
    return "";
}
