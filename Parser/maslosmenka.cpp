#include "maslosmenka.h"

/**
 * @brief MasloSmenka::MasloSmenka
 * @param parent
 */
MasloSmenka::MasloSmenka(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://www.maslosmenka.ru/";
}

/**
 * @brief MasloSmenka::search
 * @param article
 * @return
 */
QString MasloSmenka::search(Article &article)
{
    QString url = QString("https://www.maslosmenka.ru/catalog/?q=%1&spell=1&where=").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief MasloSmenka::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString MasloSmenka::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief MasloSmenka::parsePrice
 * @param block
 * @return
 */
float MasloSmenka::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<div class=\"price\"", "</div>");
    priceStr = priceStr.cut_str_to_str(">", "<");
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
 * @brief MasloSmenka::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString MasloSmenka::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<div class=\"cat-item-main\">", "</div>");
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
 * @brief MasloSmenka::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> MasloSmenka::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<!-- items-container -->";
    if (!inbuf.contains(startBlock)) {
        return blocks;
    }
    inbuf.erase_to(startBlock);
    inbuf.erase_data(1);
    inbuf = inbuf.cut_to_str(startBlock);
    QString boudoir = "data-entity=\"items-row";
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
 * @brief MasloSmenka::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString MasloSmenka::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"cat-item-main\">", "class=\"cat-buy-cont");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
