#include "toptul.h"

TopTul::TopTul(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://top-tul.ru/";
}

/**
 * @brief TopTul::search
 * @param article
 * @return
 */
QString TopTul::search(Article &article)
{
    QString url = QString("http://top-tul.ru/search/index.php?q=%1&s=").arg(article.article.trimmed());
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief TopTul::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString TopTul::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief TopTul::parsePrice
 * @param block
 * @return
 */
float TopTul::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("product-item-price-current", "</span>");
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
 * @brief TopTul::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString TopTul::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<a class=\"producttitle", ">");
    QString url = block.cut_str_to_str("href=\"", "\"");
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
 * @brief TopTul::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> TopTul::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<!-- items-container -->";
    if (!inbuf.contains(startBlock)) {
        return blocks;
    }
    inbuf.erase_to(startBlock);
    inbuf.erase_data(1);
    inbuf = inbuf.cut_to_str(startBlock);
    QString boudoir = "<div class=\"product-item-container";
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
 * @brief TopTul::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString TopTul::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"forimage vcenter\">", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"cardspecs vcenter\">", "<script>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
