#ifndef SERVISMAX_H
#define SERVISMAX_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class ServisMax : public Parser
{
    Q_OBJECT
public:
    explicit ServisMax(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
    QString doFindArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // SERVISMAX_H
