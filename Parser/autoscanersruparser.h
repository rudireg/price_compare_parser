#ifndef AUTOSCANERSRUPARSER_H
#define AUTOSCANERSRUPARSER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegExp>
#include "parser.h"

class AutoscanersruParser : public Parser
{
    Q_OBJECT
public:
    explicit AutoscanersruParser(QObject *parent = nullptr);
    QString search(Article &article) override;
    QString findBlock(QString inbuf, QString article) override;
    float parsePrice(RString block) override;
    QString parseProductUrl(RString block, QString domain) override;
protected:
    QList<QString> splitBlocks(QString inbuf);
    QString findArticleInBlock(const QList<QString> blocks, QString article);
};

#endif // AUTOSCANERSRUPARSER_H
