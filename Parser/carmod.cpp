#include "carmod.h"

/**
 * @brief Carmod::Carmod
 * @param parent
 */
Carmod::Carmod(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://www.carmod.ru/";
}

/**
 * @brief Carmod::search
 * @param article
 * @return
 */
QString Carmod::search(Article &article)
{
    this->searchList = false;
    QString url = QString("https://www.carmod.ru/products?keyword=%1").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        if (302 == this->http->httpStatusCode()) { // поиск переадресовал на конкретный товар
            this->_productUrl = this->http->Location302();
            if (this->http->get(this->_productUrl)) {
                return this->http->inbuffQString();
            }
        } else {
            this->searchList = true; // поиск отдал список
            return this->http->inbuffQString();
        }
    }
    return "";
}

/**
 * @brief Carmod::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Carmod::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Carmod::parsePrice
 * @param block
 * @return
 */
float Carmod::parsePrice(RString block)
{
    RString priceStr = "";
    // если поиск вернул список
    if (true == this->searchList) {
        block.saveToFile("bbbbb.txt");
        RString tmp = block.cut_str_to_str("<div class=\"price\">", "</div>");
        if (!tmp.isEmpty()) {
            priceStr = tmp.cut_str_to_str("<span>", "</span>");
        }
    } else { // если поиск переадресовал на конкретную страницу товара
        priceStr = block.cut_str_to_str("class=\"js-product-price\">", "</span>");
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
 * @brief Carmod::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Carmod::parseProductUrl(RString block, QString domain)
{
    QString url = "";
    // если поиск вернул список
    if (true == this->searchList) {
        block = block.cut_str_to_str("<a itemprop=\"url\"", "</a>");
        url = block.cut_str_to_str("href=\"", "\"");
        if (!url.startsWith("http") && !url.startsWith("/")) {
            url.prepend("/");
        }
    } else { // если поиск переадресовал на конкретную страницу товара
        url = this->_productUrl;
    }

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
 * @brief Carmod::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Carmod::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    // если поиск вернул список
    if (true == this->searchList) {
        QString startBlock = "id=\"catalog1\"";
        QString endBlock = "</ul>";
        if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
            return blocks;
        }
        inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
        QString boudoir = "<li class=\"product";
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
    } else { // если поиск переадресовал на конкретную страницу товара
        blocks.append(inbuf);
    }
    return blocks;
}

/**
 * @brief Carmod::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Carmod::findArticleInBlock(const QList<QString> blocks, QString article)
{
    bool isReSearch = false;
    article = article + " ";
    reSearch:
    if (isReSearch == true) {
        article = article.trimmed();
    }
    // если поиск вернул список
    if (true == this->searchList) {
        foreach(QString block, blocks) {
            RString tmp = block;
            QString title = tmp.cut_str_to_str("div class=\"ashtri", "</div>");
            if (title.contains(article)) {
                return block;
            }
        }
        foreach(QString block, blocks) {
            RString tmp = block;
            QString title = tmp.cut_str_to_str("div itemprop=\"description\"", "</div>");
            if (title.contains(article)) {
                return block;
            }
        }
    } else { // если поиск переадресовал на конкретную страницу товара
        foreach(QString block, blocks) {
            RString tmp = block;
            QString title = tmp.cut_str_to_str("<div class=\"pdoduct_heading\"", "</div>");
            if (title.contains(article)) {
                return block;
            }
        }
        foreach(QString block, blocks) {
            RString tmp = block;
            QString title = tmp.cut_str_to_str("<p itemprop=\"description\" class", "</p>");
            if (title.contains(article)) {
                return block;
            }
        }
        foreach(QString block, blocks) {
            RString tmp = block;
            QString title = tmp.cut_str_to_str("<b>Артикул:</b>", "</div>");
            if (title.contains(article)) {
                return block;
            }
        }
        foreach(QString block, blocks) {
            RString tmp = block;
            QString title = tmp.cut_str_to_str("<div itemprop=\"description\" class=\"b-tab-content\"", "id=\"Комплектация\"");
            if (title.contains(article)) {
                return block;
            }
        }
    }

    if (isReSearch == false) {
        isReSearch = true;
        goto reSearch;
    }

    return "";
}
