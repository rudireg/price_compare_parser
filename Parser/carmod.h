#ifndef CARMOD_H
#define CARMOD_H

#include <QObject>
#include <QRegExp>
#include "parser.h"

class Carmod : public Parser
{
    Q_OBJECT
public:
    explicit Carmod(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(RString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
private:
    bool searchList;
    QString _productUrl;
};

#endif // CARMOD_H
