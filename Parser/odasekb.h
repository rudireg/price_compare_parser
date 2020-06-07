#ifndef ODASEKB_H
#define ODASEKB_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class OdasEkb : public Parser
{
    Q_OBJECT
public:
    explicit OdasEkb(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QString _productUrlDetails;
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // ODASEKB_H
