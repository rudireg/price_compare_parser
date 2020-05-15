#ifndef PFTOOLRU_H
#define PFTOOLRU_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class PfToolru : public Parser
{
    Q_OBJECT
public:
    explicit PfToolru(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // PFTOOLRU_H
