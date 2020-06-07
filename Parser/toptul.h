#ifndef TOPTUL_H
#define TOPTUL_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class TopTul : public Parser
{
    Q_OBJECT
public:
    explicit TopTul(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // TOPTUL_H
