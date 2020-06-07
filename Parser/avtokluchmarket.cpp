#include "avtokluchmarket.h"

/**
 * @brief AvtokluchMarket::AvtokluchMarket
 * @param parent
 */
AvtokluchMarket::AvtokluchMarket(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://avtokluch-market.ru/";
    this->_concreateProductUrl = "";
}

/**
 * @brief AvtokluchMarket::search
 * @param article
 * @return
 */
QString AvtokluchMarket::search(Article &article)
{
    QString url = QString("http://avtokluch-market.ru/catalog/?q=%1&how=r").arg(article.article.trimmed());
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief AvtokluchMarket::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString AvtokluchMarket::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    QString result = this->findArticleInBlock(blocks, article);
    if (!result.isEmpty()) {
        this->_concreateProductUrl = QString("http://avtokluch-market.ru/catalog/?q=%1&how=r").arg(article.trimmed());
    }
    return result;
}

/**
 * @brief AvtokluchMarket::parsePrice
 * @param block
 * @return
 */
float AvtokluchMarket::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<div class=\"price", "</div>");
    if (priceStr.isEmpty()) return 0;
    priceStr.erase_to(">");
    if (priceStr.contains(",")) {
        priceStr = priceStr.cut_to_str(",");
    }
    QRegExp rx("(\\D+)");
    priceStr.replace(rx, "");
    if (priceStr.isEmpty()) return 0;
    return priceStr.toFloat();
}

/**
 * @brief AvtokluchMarket::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString AvtokluchMarket::parseProductUrl(RString block, QString domain)
{
    return this->_concreateProductUrl;
}

/**
 * @brief AvtokluchMarket::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> AvtokluchMarket::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"catalog_block";
    QString endBlock = "<div class=\"bottom_nav";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"catalog_item_wrapp";
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
 * @brief AvtokluchMarket::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString AvtokluchMarket::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"stickers", "</a>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"item-title", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    RString inb, tmp, url;
    // не нашли по артикулу из списка товаров, поэтому зайдем внутрь карточки каждого товара и там проверим совпадение
    foreach(QString block, blocks) {
        tmp = block;
        tmp = tmp.cut_str_to_str("<div class=\"item-title", "</div>");
        url = tmp.cut_str_to_str("<a href=\"", "\"");
        if (!url.isEmpty()) {
            if (!url.startsWith("http")) {
                url.prepend("http://avtokluch-market.ru");
            }
            if (this->http->get(url)) {
                inb = this->http->inbuffQString();
                inb = inb.cut_str_to_str("<div class=\"detail_text", "</li>");
                if (inb.contains(article)) {
                    return block;
                }
            }
        }
    }

    return "";
}
