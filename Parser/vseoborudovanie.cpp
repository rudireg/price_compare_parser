#include "vseoborudovanie.h"

/**
 * @brief VseOborudovanie::VseOborudovanie
 * @param parent
 */
VseOborudovanie::VseOborudovanie(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://vseoborudovanie.ru/";
}

/**
 * @brief VseOborudovanie::search
 * @param article
 * @return
 */
QString VseOborudovanie::search(Article &article)
{
    QString url = QString("http://vseoborudovanie.ru/search/?text=%1&submit=Найти").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief VseOborudovanie::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString VseOborudovanie::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief VseOborudovanie::parsePrice
 * @param block
 * @return
 */
float VseOborudovanie::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<span class=\"catalog-item-price-discount\">", "<span>");
    if (priceStr.isEmpty()) {
        priceStr = block.cut_str_to_str("<span class=\"catalog-item-price\">", "<span>");
    }
    if (priceStr.isEmpty()) {
        priceStr = block.cut_str_to_str("<span class=\"catalog-item-price-old\">", "<span>");
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
 * @brief VseOborudovanie::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString VseOborudovanie::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<div class=\"catalog-item-title\">", "</div>");
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
 * @brief VseOborudovanie::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> VseOborudovanie::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"catalog-item-cards\"";
    QString endBlock = "<div class=\"clr\"></div>";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"catalog-item\"";
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
 * @brief VseOborudovanie::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString VseOborudovanie::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<img class=\"item_img\"", "/>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("itemprop=\"shop_item/name\">", "</span>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
