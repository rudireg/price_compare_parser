#ifndef OPTIMATOOLS_H
#define OPTIMATOOLS_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class OptimaTools : public Parser
{
    Q_OBJECT
public:
    explicit OptimaTools(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QString _cartProductUrl;
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // OPTIMATOOLS_H
