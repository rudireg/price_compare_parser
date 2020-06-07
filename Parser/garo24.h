#ifndef GARO24_H
#define GARO24_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class Garo24 : public Parser
{
    Q_OBJECT
public:
    explicit Garo24(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
    QString gotoProductPage(RString block);
};

#endif // GARO24_H
