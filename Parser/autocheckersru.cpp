#include "autocheckersru.h"

AutoCheckersRu::AutoCheckersRu(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://autocheckers.ru/";
}

/**
 * @brief AutoCheckersRu::search
 * @param article
 * @return
 */
QString AutoCheckersRu::search(Article &article)
{
    QString searchUrl = QString("https://autocheckers.ru/search/?search=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = searchUrl;
    if (this->http->get(searchUrl)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief AutoCheckersRu::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString AutoCheckersRu::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief AutoCheckersRu::parsePrice
 * @param block
 * @return
 */
float AutoCheckersRu::parsePrice(RString block)
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
 * @brief AutoCheckersRu::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString AutoCheckersRu::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<h2>", "</h2>");
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
 * @brief AutoCheckersRu::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> AutoCheckersRu::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString boudoir = "product-layout";
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
 * @brief AutoCheckersRu::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString AutoCheckersRu::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;

        QString title = tmp.cut_str_to_str("<h2>", "</h2>");
        if (title.contains(article)) {
            return block;
        }

        title = tmp.cut_str_to_str("<p>", "</p>");
        if (title.contains(article)) {
            return block;
        }
    }
    return "";
}
