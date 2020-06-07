#include "mnogotools.h"

/**
 * @brief Mnogotools::Mnogotools
 * @param parent
 */
Mnogotools::Mnogotools(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://mnogotools.ru/";
}

/**
 * @brief Mnogotools::search
 * @param article
 * @return
 */
QString Mnogotools::search(Article &article)
{
    QString url = QString("https://mnogotools.ru/search/?sort=p.price&order=ASC&search=%1&description=true&category_id=265&sub_category=true").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief Mnogotools::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Mnogotools::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Mnogotools::parsePrice
 * @param block
 * @return
 */
float Mnogotools::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<div class=\"price\">", "</div>");
    if (priceStr.isEmpty()) return 0;
    if (priceStr.contains(".")) {
        priceStr = priceStr.cut_to_str(".");
    }
    if (priceStr.contains(",")) {
        priceStr = priceStr.cut_to_str(",");
    }
    QRegExp rx("(\\D+)");
    priceStr.replace(rx, "");
    if (priceStr.isEmpty()) return 0;
    return priceStr.toFloat();
}

/**
 * @brief Mnogotools::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Mnogotools::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<h4", "</h4>");
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
 * @brief Mnogotools::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Mnogotools::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"row main-products";
    QString endBlock = "<div class=\"row pagination\">";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"product-grid-item";
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
 * @brief Mnogotools::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Mnogotools::findArticleInBlock(const QList<QString> blocks, QString article)
{
    QString block = this->concreteFindArticleInBlock(blocks, article + " ");
    if (!block.isEmpty()) {
        return block;
    }

    block = this->concreteFindArticleInBlock(blocks, article);
    if (!block.isEmpty()) {
        return block;
    }

    if (article.contains("_")) {
        block = this->concreteFindArticleInBlock(blocks, article.replace("_", "&"));
        if (!block.isEmpty()) {
            return block;
        }
    }

    return "";
}

/**
 * @brief Mnogotools::concreteFindArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Mnogotools::concreteFindArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<h4", "</h4>");
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
