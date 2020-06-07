#ifndef MNOGOTOOLS_H
#define MNOGOTOOLS_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class Mnogotools : public Parser
{
    Q_OBJECT
public:
    explicit Mnogotools(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
    QString concreteFindArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // MNOGOTOOLS_H
