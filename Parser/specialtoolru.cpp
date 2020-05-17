#include "specialtoolru.h"

SpecialToolRu::SpecialToolRu(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://special-tool.ru/";
}

/**
 * @brief SpecialToolRu::search
 * @param article
 * @return
 */
QString SpecialToolRu::search(Article &article)
{
    QString searchUrl = QString("https://special-tool.ru/catalog/?q=%1&s=").arg(article.article);
    article.sites[this->siteIndex].searchUrl = searchUrl;
    if (this->http->get(searchUrl)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief SpecialToolRu::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString SpecialToolRu::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief SpecialToolRu::parsePrice
 * @param block
 * @return
 */
float SpecialToolRu::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("<span class=\"value\">", "</span>");
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
 * @brief SpecialToolRu::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString SpecialToolRu::parseProductUrl(RString block, QString domain)
{
    block.erase_to("<div class=\"name\">");
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
 * @brief SpecialToolRu::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> SpecialToolRu::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString boudoir = "catalog-item-wrap active";
    inbuf = inbuf.cut_to_str("<span class=\"current-state\">");
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
 * @brief SpecialToolRu::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString SpecialToolRu::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;

        QString title = tmp.cut_str_to_str("<div class=\"name\">", "</div>");
        if (title.contains(article)) {
            return block;
        }

        title = tmp.cut_str_to_str("<span class=\"art\">", "</span>");
        if (title.contains(article)) {
            return block;
        }
    }
    return "";
}
