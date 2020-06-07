#include "arkudateh.h"

/**
 * @brief Arkudateh::Arkudateh
 * @param parent
 */
Arkudateh::Arkudateh(QObject *parent) : Parser(parent)
{
    this->siteUrl = "http://arkudateh.ru/";
}

/**
 * @brief Arkudateh::search
 * @param article
 * @return
 */
QString Arkudateh::search(Article &article)
{
    this->_direct = false;
    this->_concreteProductUrl = "";
    QString url = QString("http://arkudateh.ru/?s=%1&post_type=product").arg(article.article.trimmed());
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        if (this->http->httpStatusCode() == 302) {
            QString url = this->http->Location302();
            if (this->http->get(url)) {
                this->_direct = true;
                this->_concreteProductUrl = url;
                return this->http->inbuffQString();
            }
        }
        return this->http->inbuffQString();
    }

    return "";
}

/**
 * @brief Arkudateh::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString Arkudateh::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief Arkudateh::parsePrice
 * @param block
 * @return
 */
float Arkudateh::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("woocommerce-Price-amount", "<span");
    if (priceStr.isEmpty()) return 0;
    if (priceStr.contains(".")) {
        priceStr = priceStr.cut_to_str(".");
    }
    if (priceStr.contains(",")) {
        priceStr = priceStr.cut_to_str(",");
    }
    QRegExp rx("(\\D+)");
    priceStr.replace(rx, "");
    if (priceStr.isEmpty()) return 0;
    return priceStr.toFloat();
}

/**
 * @brief Arkudateh::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString Arkudateh::parseProductUrl(RString block, QString domain)
{
    if (this->_direct == true) {
        return this->_concreteProductUrl;
    }
    block = block.cut_str_to_str("class=\"nxowoo-box\">", "</span>");
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
 * @brief Arkudateh::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> Arkudateh::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    if (this->_direct == true) {
        inbuf = inbuf.cut_str_to_str("<div id=\"product", "data-mode=\"share\"");
        if (!inbuf.isEmpty()) {
            blocks.append(inbuf);
        }
        return blocks;
    }
    QString startBlock = "<ul class=\"products";
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
    return blocks;
}

/**
 * @brief Arkudateh::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Arkudateh::findArticleInBlock(const QList<QString> blocks, QString article)
{
    if (this->_direct == true) {
        QString result = this->findCardArticleInBlock(blocks, article);
        if (!result.isEmpty()) {
            return result;
        }
    } else {
        QString result = "";
        QString block = this->findConcreteArticleInBlock(blocks, article + " ");
        if (!block.isEmpty()) {
            result = block;
        }
        if (result.isEmpty()) {
            block = this->findConcreteArticleInBlock(blocks, article + "<");
            if (!block.isEmpty()) {
                result = block;
            }
        }
        if (result.isEmpty()) {
            block = this->findConcreteArticleInBlock(blocks, article + ".");
            if (!block.isEmpty()) {
                result = block;
            }
        }
        if (result.isEmpty()) {
            block = this->findConcreteArticleInBlock(blocks, article + "&");
            if (!block.isEmpty()) {
                result = block;
            }
        }
        // перейдем на карточку товара
        if (!result.isEmpty()) {
            QString url = this->parseProductUrl(result, "http://arkudateh.ru/");
            this->_direct = true;
            this->_concreteProductUrl = url;
            if (!url.isEmpty() && this->http->get(url)) {
                RString inbuf = this->http->inbuffQString();
                QString title = inbuf.cut_str_to_str("<span class=\"sku\">", "</span>");
                if (title == article) {
                    return inbuf.toUtf8();
                }
            }
        }
    }

    return "";
}

/**
 * @brief Arkudateh::findCardArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Arkudateh::findCardArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<span class=\"sku\">", "</span>");
        if (title == article) {
            return block;
        }
    }

    return "";
}

/**
 * @brief Arkudateh::findConcreteArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString Arkudateh::findConcreteArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("<h2 ", "</h2>");
        if (title.contains(article)) {
            return block;
        }
    }

    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("woocommerce-product-details__short-description", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
