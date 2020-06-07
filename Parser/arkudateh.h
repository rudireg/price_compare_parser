#ifndef ARKUDATEH_H
#define ARKUDATEH_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class Arkudateh : public Parser
{
    Q_OBJECT
public:
    explicit Arkudateh(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    bool _direct;
    QString _concreteProductUrl;
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
    QString findConcreteArticleInBlock(const QList<QString> blocks, QString article);
    QString findCardArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // ARKUDATEH_H
