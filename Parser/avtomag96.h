#ifndef AVTOMAG96_H
#define AVTOMAG96_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class Avtomag96 : public Parser
{
    Q_OBJECT
public:
    explicit Avtomag96(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // AVTOMAG96_H
