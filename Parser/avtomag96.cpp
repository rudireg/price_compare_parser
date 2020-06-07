#include "avtomag96.h"

/**
 * @brief Avtomag96::Avtomag96
 * @param parent
 */
Avtomag96::Avtomag96(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://avtomag96.ru/";
}

/**
 * @brief Avtomag96::search
 * @param article
 * @return
 */
QString Avtomag96::search(Article &article)
{
    QString url = QString("https://avtomag96.ru/catalog/search.html?s=%1&sf=1").arg(article.article.trimmed());
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief Avtomag96::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Avtomag96::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Avtomag96::parsePrice
 * @param block
 * @return
 */
float Avtomag96::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<span class=\"item-price", "</span>");
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
 * @brief Avtomag96::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Avtomag96::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<div class=\"item-header", "<div class=\"item-footer");
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
 * @brief Avtomag96::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Avtomag96::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"item-list\">";
    QString endBlock = "<script>";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"item\" id=";
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
 * @brief Avtomag96::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Avtomag96::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("data-name=\"", "\"");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"item-header", "<div class=\"item-footer");
        if (title.contains(article)) {
            return block;
        }
    }

    if (article.contains("_")) {
        article = article.replace("_", "&amp;");

        foreach(QString block, blocks) {
            RString tmp = block;
            QString title = tmp.cut_str_to_str("data-name=\"", "\"");
            if (title.contains(article)) {
                return block;
            }
        }

        foreach(QString block, blocks) {
            RString tmp = block;
            QString title = tmp.cut_str_to_str("<div class=\"item-header", "<div class=\"item-footer");
            if (title.contains(article)) {
                return block;
            }
        }
    }

    return "";
}
