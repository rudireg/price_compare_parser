#include "rustehnikaruparser.h"

RustehnikaRuParser::RustehnikaRuParser(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://www.rustehnika.ru/";
}

/**
 * @brief RustehnikaRuParser::search
 * @param article
 * @return
 */
QString RustehnikaRuParser::search(Article &article)
{
    QString url = QString("https://www.rustehnika.ru/search/?s=&q=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief RustehnikaRuParser::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString RustehnikaRuParser::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief RustehnikaRuParser::parsePrice
 * @param block
 * @return
 */
float RustehnikaRuParser::parsePrice(RString block)
{
    QString boudoir = "<span class=\"b-catalog__item-price-num\">";
    QString priceStr = block.cut_str_to_str(boudoir, "</span>");
    if (priceStr.isEmpty()) return 0;
    QRegExp rx("(\\D+)");
    priceStr.replace(rx, "");
    if (priceStr.isEmpty()) return 0;
    return priceStr.toFloat();
}

/**
 * @brief RustehnikaRuParser::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> RustehnikaRuParser::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString boudoir = "<div class=\"categories__list_wide\">";
    while (inbuf.contains(boudoir)) {
        QString block = inbuf.cut_str_to_str(boudoir, "<noindex>");
        if (!block.isEmpty()) {
            blocks.append(block);
        }
        inbuf.erase_to(boudoir);
        inbuf.erase_data(1);
    }
    return blocks;
}

/**
 * @brief RustehnikaRuParser::findArticleInBlock
 * @param blocks
 * @return
 */
QString RustehnikaRuParser::findArticleInBlock(QList<QString> const blocks, QString article)
{
    QString needle = QString("<strong>%1</strong>").arg(article);
    foreach(QString block, blocks) {
        if (block.contains(needle)) {
            return block;
        }
    }
    return "";
}

/**
 * @brief RustehnikaRuParser::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString RustehnikaRuParser::parseProductUrl(RString block, QString domain)
{
    QString title("b-catalog__item-title");
    if (!block.contains(title)) return "";
    block.erase_to(title);
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
