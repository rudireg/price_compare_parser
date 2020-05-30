#include "terminaltools.h"

/**
 * @brief TerminalTools::TerminalTools
 * @param parent
 */
TerminalTools::TerminalTools(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://terminal-tools.ru/";
}

/**
 * @brief TerminalTools::search
 * @param article
 * @return
 */
QString TerminalTools::search(Article &article)
{
    QString url = QString("http://terminal-tools.ru/search/?searchstring=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief TerminalTools::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString TerminalTools::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief TerminalTools::parsePrice
 * @param block
 * @return
 */
float TerminalTools::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<span class=\"totalPrice\">", "</span>");
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
 * @brief TerminalTools::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString TerminalTools::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<div class=\"prdbrief_name\">", "</div>");
    QString url = block.cut_str_to_str("href='", "'");
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
 * @brief TerminalTools::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> TerminalTools::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "id=\"cat_product_sort\"";
    QString endBlock = "cpt_container_end";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<td valign=top";
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
 * @brief TerminalTools::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString TerminalTools::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"prdbrief_name\">", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("prdbrief_thumbnail", "</table>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
