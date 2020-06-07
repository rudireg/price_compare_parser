#include "optimatools.h"

/**
 * @brief OptimaTools::OptimaTools
 * @param parent
 */
OptimaTools::OptimaTools(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://optima-tools.ru/";
}

/**
 * @brief OptimaTools::search
 * @param article
 * @return
 */
QString OptimaTools::search(Article &article)
{
    QString url = QString("http://optima-tools.ru/search/search_do/?search_string=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief OptimaTools::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString OptimaTools::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief OptimaTools::parsePrice
 * @param block
 * @return
 */
float OptimaTools::parsePrice(RString block)
{
    RString priceStr = "";
    if (block.contains("<span class=\"price price_value price_with_discount\">")) {
        priceStr = block.cut_str_to_str("<span class=\"price price_value price_with_discount\">", "</span>");
    }
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
 * @brief OptimaTools::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString OptimaTools::parseProductUrl(RString block, QString domain)
{
    return this->_cartProductUrl;
}

/**
 * @brief OptimaTools::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> OptimaTools::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"search\">";
    QString endBlock = "<strong>Всего найдено";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div>";
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
 * @brief OptimaTools::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString OptimaTools::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        if (block.contains(article)) {
            RString url = block;
            url = url.cut_str_to_str("<a href=\"", "\"");
            if (!url.isEmpty()) {
                url = "http://optima-tools.ru" + url;
                this->_cartProductUrl = url;
                if (this->http->get(url)) {
                    return this->http->inbuffQString();
                }
            }
        }
    }

    return "";
}
