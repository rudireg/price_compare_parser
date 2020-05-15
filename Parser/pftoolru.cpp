#include "pftoolru.h"

PfToolru::PfToolru(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://pf-tool.ru/";
}

/**
 * @brief PfToolru::search
 * @param article
 * @return
 */
QString PfToolru::search(Article &article)
{
    QString url = QString("http://pf-tool.ru/catalog/index.php?sort=PRICE&order=desc&q=%1&how=r").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief PfToolru::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString PfToolru::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief PfToolru::parsePrice
 * @param block
 * @return
 */
float PfToolru::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<span class=\"price_value\">", "</span>");
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
 * @brief PfToolru::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString PfToolru::parseProductUrl(RString block, QString domain)
{
    RString titleBlock = block.cut_str_to_str("<div class=\"item-title", "</div>");
    QString url = titleBlock.cut_str_to_str("<a href=\"", "\"");
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
 * @brief PfToolru::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> PfToolru::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "top_wrapper row margin0 show_un_props";
    QString endBlock = "<div class=\"bottom_nav";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "catalog_item_wrapp";
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
 * @brief PfToolru::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString PfToolru::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"article_block\">", "</div>");
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

    return "";
}
