#include "arstools.h"

/**
 * @brief Arstools::Arstools
 * @param parent
 */
Arstools::Arstools(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://arstools.ru/";
}

/**
 * @brief Arstools::search
 * @param article
 * @return
 */
QString Arstools::search(Article &article)
{
    QString params = QString("Find=1&w=2&GoPr=1&ajaxSearch=&query=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = QString("https://arstools.ru/modules/ajax_search.php?query=%1").arg(article.article);
    if (this->http->post("https://arstools.ru/?action=catalog", params.toUtf8())) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief Arstools::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Arstools::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Arstools::parsePrice
 * @param block
 * @return
 */
float Arstools::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("type=\"hidden\" name=\"price\" value=\"", "\"");
    priceStr.replace(".00", "");
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
 * @brief Arstools::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Arstools::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("<div class=\"item-name\">", "</a>");
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
 * @brief Arstools::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Arstools::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"row items list-view\">";
    QString endBlock = "<div class=\"col-sm-12 products\">";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"spacer\">";
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
 * @brief Arstools::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Arstools::findArticleInBlock(const QList<QString> blocks, QString article)
{
    QString block = this->tryFindArticle(blocks, article + " ");
    if (!block.isEmpty()) {
        return block;
    }

    block = this->tryFindArticle(blocks, article + "\"");
    if (!block.isEmpty()) {
        return block;
    }

    block = this->tryFindArticle(blocks, article);
    if (!block.isEmpty()) {
        return block;
    }

    return "";
}

/**
 * @brief Arstools::tryFindArticle
 * @param blocks
 * @param article
 * @return
 */
QString Arstools::tryFindArticle(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<div class=\"item-img\">", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<h5 class=\"modal-title\">", "</h5>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
