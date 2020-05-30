#include "avtomirvologda.h"

/**
 * @brief AvtomirVologda::AvtomirVologda
 * @param parent
 */
AvtomirVologda::AvtomirVologda(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://avtomir-vologda.ru/";
}

/**
 * @brief AvtomirVologda::search
 * @param article
 * @return
 */
QString AvtomirVologda::search(Article &article)
{
    QString url = QString("http://avtomir-vologda.ru/search/?sort=p.price&order=ASC&search=%1&description=true").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief AvtomirVologda::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString AvtomirVologda::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief AvtomirVologda::parsePrice
 * @param block
 * @return
 */
float AvtomirVologda::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<span class=\"common-price\">", "</span>");
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
 * @brief AvtomirVologda::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString AvtomirVologda::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<h4>", "</h4>");
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
 * @brief AvtomirVologda::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> AvtomirVologda::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div id=\"res-products\">";
    QString endBlock = "<footer>";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "div class=\"product-layout";
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
 * @brief AvtomirVologda::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString AvtomirVologda::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<p class=\"cat-model\">", "</p>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<h4>", "</h4>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<p class=\"oct-product-desc\">", "</p>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
