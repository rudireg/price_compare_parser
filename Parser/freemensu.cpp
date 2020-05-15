#include "freemensu.h"

/**
 * @brief FreeMenSu::FreeMenSu
 * @param parent
 */
FreeMenSu::FreeMenSu(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://www.freemen.su/";
}

/**
 * @brief FreeMenSu::search
 * @param article
 * @return
 */
QString FreeMenSu::search(Article &article)
{
    QString url = QString("http://freemen.su/search/?sort=p.price&order=ASC&description=true&search=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief FreeMenSu::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString FreeMenSu::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief FreeMenSu::parsePrice
 * @param block
 * @return
 */
float FreeMenSu::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<div class=\"price\">", "</div>");
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
 * @brief FreeMenSu::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString FreeMenSu::parseProductUrl(RString block, QString domain)
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
 * @brief FreeMenSu::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> FreeMenSu::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"product-list\">";
    QString endBlock = "<div class=\"pagination\">";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "for swap image";
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
 * @brief FreeMenSu::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString FreeMenSu::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"name\">", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"description\">", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    // если не нашли, идем на страницу каждого товара и ищем артикуль там
    foreach(QString block, blocks) {
        QString productUrl = this->parseProductUrl(block, this->siteUrl);
        if (!productUrl.isEmpty()) {
            if (this->http->get(productUrl)) {
                RString inb = this->http->inbuffQString();
                QString description = inb.cut_str_to_str("<div class=\"description\">", "</div>");
                if (description.contains(article)) {
                    return block;
                }
            }
        }
    }

    return "";
}
