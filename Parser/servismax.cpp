#include "servismax.h"
#include "servismax.h"

/**
 * @brief ServisMax::ServisMax
 * @param parent
 */
ServisMax::ServisMax(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://www.servismax.ru/";
}

/**
 * @brief ServisMax::search
 * @param article
 * @return
 */
QString ServisMax::search(Article &article)
{
    QString url = QString("http://www.servismax.ru/new/shop/search/?query=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief ServisMax::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString ServisMax::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief ServisMax::parsePrice
 * @param block
 * @return
 */
float ServisMax::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("class=\"price nowrap\">", "</span>");
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
 * @brief ServisMax::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString ServisMax::parseProductUrl(RString block, QString domain)
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
 * @brief ServisMax::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> ServisMax::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<ul class=\"product-list thumbs\">";
    QString endBlock = "</ul>";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<li itemscope itemtype";
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
 * @brief ServisMax::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString ServisMax::findArticleInBlock(const QList<QString> blocks, QString article)
{
    QString block;

    article = article + " ";
    block = this->doFindArticleInBlock(blocks, article);
    if (!block.isEmpty()) {
        return block;
    }

    article = article.replace("-", " ");
    block = this->doFindArticleInBlock(blocks, article);
    if (!block.isEmpty()) {
        return block;
    }

    article = article.replace(" ", "");
    article = article + " ";
    block = this->doFindArticleInBlock(blocks, article);
    if (!block.isEmpty()) {
        return block;
    }

    return "";
}

/**
 * @brief ServisMax::doFindArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString ServisMax::doFindArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("\" title=\"", "\">");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<meta itemprop=\"description\"", "\">");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"name\">", "\">");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<h5 itemprop=\"name\"", "</h5>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("itemprop=\"description\">", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("data-name = \"", "\"");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
