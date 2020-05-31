#include "imperiyaavto43.h"

/**
 * @brief Imperiyaavto43::Imperiyaavto43
 * @param parent
 */
Imperiyaavto43::Imperiyaavto43(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://imperiyaavto43.ru/";
}

/**
 * @brief Imperiyaavto43::search
 * @param article
 * @return
 */
QString Imperiyaavto43::search(Article &article)
{
    QString url = QString("https://imperiyaavto43.ru/site_search?search_term=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
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
