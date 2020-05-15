#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "Network/rhttp.h"
#include "rstring.h"

enum class ParserStatus: int {
    WAIT,
    DONE,
    WARNING_ARTICLE_NOT_FOUND,
    ERROR_EMPTY_INBUF,
    ERROR_PRICE_NOT_FOUND,
    ERROR_PRODUCT_URL_NOT_FOUND
};

struct Site {
    Site () {
        this->url.clear();
        this->price = 0;
        this->searchUrl.clear();
        this->productUrl.clear();
        this->parserStatus = ParserStatus::WAIT;
        this->errorMessage.clear();
    }
    QString url;
    int price;
    QString searchUrl;
    QString productUrl;
    ParserStatus parserStatus;
    QString errorMessage;
};

struct Article {
    Article() {
        this->article.clear();
    }
    void clear() {
        this->article.clear();
        this->sites.clear();
    }
    QString article;
    QList<Site> sites;
};

/**
 * @brief The Parser class
 */
class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject *parent = nullptr);
    ~Parser();
    virtual QString search(Article &article) = 0;
    virtual QString findBlock(QString inbuf, QString article) = 0;
    virtual float parsePrice(RString block) = 0;
    virtual QString parseProductUrl(RString block, QString domain) = 0;
    void process(Article &article);

protected:
    RHttp *http;
    QString siteUrl;
    int siteIndex;
    int findSite(QList<Site> &sites, QString url);
signals:
    void addTableStatus(int idThread, QString article, QString status, QString color);
};

#endif // PARSER_H
