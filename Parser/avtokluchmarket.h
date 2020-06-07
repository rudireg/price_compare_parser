#ifndef AVTOKLUCHMARKET_H
#define AVTOKLUCHMARKET_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class AvtokluchMarket  : public Parser
{
    Q_OBJECT
public:
    explicit AvtokluchMarket(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QString _concreateProductUrl;
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // AVTOKLUCHMARKET_H
