#include "sl33.h"

Sl33::Sl33(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://sl33.ru/";
}

/**
 * @brief Sl33::search
 * @param article
 * @return
 */
QString Sl33::search(Article &article)
{

    QString url = QString("https://yandex.ru/search/site/?searchid=2237329&text=\"%1\"&web=0&l10n=ru").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief Sl33::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Sl33::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Sl33::parsePrice
 * @param block
 * @return
 */
float Sl33::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("class=\"price nowrap\">", "</span>");
    if (priceStr.isEmpty()) return 0;
    if (priceStr.contains(",")) {
        priceStr = priceStr.cut_to_str(",");
    }
    QRegExp rx("(\\D+)");
    priceStr.replace(rx, "");
    if (priceStr.isEmpty()) return 0;
    return priceStr.toFloat();
}

/**  property="og:url" content="https://sl33.ru/apparat-dlja-promyvki-toplivnoj-sistemy-gx-100/" />
 * @brief Sl33::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Sl33::parseProductUrl(RString block, QString domain)
{
    QString url = block.cut_str_to_str("property=\"og:url\" content=\"", "\"");
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
 * @brief Sl33::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Sl33::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    inbuf = inbuf.cut_str_to_str("<ol class=\"b-serp-list\">", "</ol>");
    QString boudoir = "<li class=\"b-serp-item\">";
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
 * @brief Sl33::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Sl33::findArticleInBlock(const QList<QString> blocks, QString article)
{
    QString clearArticle = article;
    clearArticle.replace("-", "");
    foreach(QString block, blocks) {
        RString tmp = block;
        QString url = tmp.cut_str_to_str("href=\"", "\"");
        if (!url.isEmpty()) {
            if (this->http->get(url)) {
                RString inbuf = this->http->inbuffQString();
                RString title = inbuf.cut_str_to_str("<h1 itemprop=\"name\">", "</h1>");
                if (title.contains(article) || title.contains(clearArticle)) {
                    return QString(inbuf);
                }
                title = inbuf.cut_str_to_str("Артикул: <span>", "</span>");
                if (title.contains(article) || title.contains(clearArticle)) {
                    return QString(inbuf);
                }
                title = inbuf.cut_str_to_str("<form id=\"cart-form\"", "</form>");
                title = title.cut_str_to_str("<p style=\"text-align: justify;\">", "</p>");
                if (title.contains(article) || title.contains(clearArticle)) {
                    return QString(inbuf);
                }
            }
        }
    }
    return "";
}
