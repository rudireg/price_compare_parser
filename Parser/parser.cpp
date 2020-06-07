#include "parser.h"

Parser::Parser(QObject *parent) : QObject(parent)
{
    this->http = new RHttp();
    this->setUsdRate(1);
}

Parser::~Parser()
{
    this->http->deleteLater();
}

/**
 * @brief Parser::process
 * @param article
 */
void Parser::process(Article &article)
{
    this->siteIndex = this->findSite(article.sites, this->siteUrl);
    emit addTableStatus(this->objectName().toInt(), article.article, QString("Парсим %1").arg(this->siteUrl), "green");

    article.article = article.article.trimmed();
    // search
    QString inb = this->search(article);
    if (inb.isEmpty()) {
        article.sites[this->siteIndex].price = 0;
        article.sites[this->siteIndex].parserStatus = ParserStatus::ERROR_EMPTY_INBUF;
        article.sites[this->siteIndex].errorMessage = "Поиск вернул пустой ответ";
        return;
    }
    // find product block
    QString block = this->findBlock(inb, article.article.trimmed());
    if (block.isEmpty()) {
        article.sites[this->siteIndex].parserStatus = ParserStatus::WARNING_ARTICLE_NOT_FOUND;
        article.sites[this->siteIndex].errorMessage = "Артикул не найден";
        return;
    }
    // parse price
    article.sites[this->siteIndex].price = this->parsePrice(block);
    if (article.sites[this->siteIndex].price < 1) {
        article.sites[this->siteIndex].parserStatus = ParserStatus::ERROR_PRICE_NOT_FOUND;
        article.sites[this->siteIndex].errorMessage = "Цена не найдена";
        return;
    }
    // parse product URL
    article.sites[this->siteIndex].productUrl = this->parseProductUrl(block, this->siteUrl);
    if (article.sites[this->siteIndex].productUrl.isEmpty()) {
        article.sites[this->siteIndex].parserStatus = ParserStatus::ERROR_PRODUCT_URL_NOT_FOUND;
        article.sites[this->siteIndex].errorMessage = "Ссылка на продукт не найдена";
        return;
    }
    // done
    article.sites[this->siteIndex].parserStatus = ParserStatus::DONE;
}

/**
 * @brief Parser::findSite
 * @param sites
 * @param url
 * @return
 */
int Parser::findSite(QList<Site>& sites, QString url)
{
    QList<Site>::iterator i;
    for (i = sites.begin(); i != sites.end(); ++i) {
        if (i->url == url) {
            return i - sites.begin();
        }
    }
    return -1;
}
