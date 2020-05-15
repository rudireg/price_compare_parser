#include "autoscanersruparser.h"

AutoscanersruParser::AutoscanersruParser(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://www.autoscaners.ru/";
}

/**
 * @brief AutoscanersruParser::search
 * @param article
 * @return
 */
QString AutoscanersruParser::search(Article &article)
{
    QString url = QString("https://api.multisearch.io/?id=10593&lang=ru&m=1589107292157&q=8o3kkk&query=%1&s=large").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief AutoscanersruParser::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString AutoscanersruParser::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief AutoscanersruParser::parsePrice
 * @param block
 * @return
 */
float AutoscanersruParser::parsePrice(RString block)
{
    QJsonDocument doc = QJsonDocument::fromJson(block.toUtf8());
    QJsonObject jsonObject = doc.object();
    return jsonObject.value("price").toString().toFloat();
}

/**
 * @brief AutoscanersruParser::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString AutoscanersruParser::parseProductUrl(RString block, QString domain)
{
    QJsonDocument doc = QJsonDocument::fromJson(block.toUtf8());
    QJsonObject jsonObject = doc.object();
    QString url = jsonObject.value("url").toString();
    if (url.isEmpty()) return "";
    if (!url.startsWith("http")) {
        if (domain.endsWith("/")) {
            url.remove(0, 1);
        }
        url.prepend(domain);
    }
    return url;
}


/**
 * @brief RustehnikaRuParser::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> AutoscanersruParser::splitBlocks(QString inbuf)
{
    QList<QString> blocks;
    QJsonDocument doc = QJsonDocument::fromJson(inbuf.toUtf8());
    if (!doc.isEmpty()) {
        QJsonObject jsonObject = doc.object();
        QJsonValue value = jsonObject.value("results").toObject().value("item_groups");
        foreach (const QJsonValue value, value.toArray()) {
            if (value["items"][0].isArray()) {
                QJsonDocument docRes(value["items"][0][0].toObject());
                blocks.append(docRes.toJson(QJsonDocument::Compact));
            } else {
                QJsonDocument docRes(value["items"][0].toObject());
                blocks.append(docRes.toJson(QJsonDocument::Compact));
            }
        }
    }
    return blocks;
}

/**
 * @brief RustehnikaRuParser::findArticleInBlock
 * @param blocks
 * @return
 */
QString AutoscanersruParser::findArticleInBlock(QList<QString> const blocks, QString article)
{
    foreach(QString block, blocks) {
        QJsonDocument doc = QJsonDocument::fromJson(block.toUtf8());
        QJsonObject jsonObject = doc.object();
        if (jsonObject.value("name").toString().contains(article)) {
            return block;
        }
    }
    return "";
}














