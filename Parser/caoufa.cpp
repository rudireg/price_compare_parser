#include "caoufa.h"

CaoUfa::CaoUfa(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://cao-ufa.ru/";
}

/**
 * @brief CaoUfa::search
 * @param article
 * @return
 */
QString CaoUfa::search(Article &article)
{
    QString url = QString("https://cao-ufa.ru/component/search/?searchword=%1&searchwordsugg=&option=com_search").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief CaoUfa::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString CaoUfa::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief CaoUfa::parsePrice
 * @param block
 * @return
 */
float CaoUfa::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("itemprop=price>", "<");
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
 * @brief CaoUfa::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString CaoUfa::parseProductUrl(RString block, QString domain)
{
    RString url = block.cut_str_to_str("<base href", "/>");
    url.erase_data(1);
    if (url.isEmpty()) return "";
    if (!url.startsWith("http")) {
        if (domain.endsWith("/")) {
            url.remove(0, 1);
        }
        url.prepend(domain);
    }
    return url.toUtf8();
}

/**
 * @brief CaoUfa::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> CaoUfa::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<dl class=search-results>";
    QString endBlock = "class=pagination";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "dt class=result-title";
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
 * @brief CaoUfa::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString CaoUfa::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("a href=", "</a>");
        if (title.contains(article)) {
            QString url = "https://cao-ufa.ru" + tmp.cut_str_to_str("a href=", ">");
            if (this->http->get(url)) {
                return this->http->inbuffQString();
            }
            break;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("class=result-text", "</dd>");
        if (title.contains(article)) {
            QString url = "https://cao-ufa.ru" + tmp.cut_str_to_str("a href=", ">");
            if (this->http->get(url)) {
                return this->http->inbuffQString();
            }
            break;
        }
    }

    return "";
}
