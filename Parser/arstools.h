#ifndef ARSTOOLS_H
#define ARSTOOLS_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class Arstools : public Parser
{
    Q_OBJECT
public:
    explicit Arstools(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
    QString tryFindArticle(const QList<QString> blocks, QString article);
};

#endif // ARSTOOLS_H
