#include "servicequipment.h"

/**
 * @brief ServiceQuipment::ServiceQuipment
 * @param parent
 */
ServiceQuipment::ServiceQuipment(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://www.servicequipment.ru/";
}

/**
 * @brief ServiceQuipment::search
 * @param article
 * @return
 */
QString ServiceQuipment::search(Article &article)
{
    QString url = QString("https://www.servicequipment.ru/catalog/?q=%1&spell=1&where=").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief ServiceQuipment::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString ServiceQuipment::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief ServiceQuipment::parsePrice
 * @param block
 * @return
 */
float ServiceQuipment::parsePrice(RString block)
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
 * @brief ServiceQuipment::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString ServiceQuipment::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<div class=\"item-title\">", "</div>");
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
 * @brief ServiceQuipment::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> ServiceQuipment::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"display_list show_un_props\">";
    QString endBlock = "class=\"bottom_nav list\"";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"list_item_wrapp";
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
 * @brief ServiceQuipment::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString ServiceQuipment::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"article_block\"", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"item-title\">", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
