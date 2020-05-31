#include "scan2.h"

/**
 * @brief Scan2::Scan2
 * @param parent
 */
Scan2::Scan2(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://scan2.ru/";
}

/**
 * @brief Scan2::search
 * @param article
 * @return
 */
QString Scan2::search(Article &article)
{
    QString url = QString("https://scan2.ru/search?search=%1&sub_category=true&description=true").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief Scan2::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Scan2::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Scan2::parsePrice
 * @param block
 * @return
 */
float Scan2::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<p class=\"price\">", "</p>");
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
 * @brief Scan2::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Scan2::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<div class=\"name\">", "</div>");
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
 * @brief Scan2::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Scan2::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<h2>Результаты поиска</h2>";
    QString endBlock = "<div class=\"row pagination-wrapper\">";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"product-layout";
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
 * @brief Scan2::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Scan2::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("\" title=\"", "\"");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"name\">", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<p class=\"description\">", "</p>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
