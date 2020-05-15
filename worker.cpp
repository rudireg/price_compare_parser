#include "worker.h"

Worker::Worker(QObject *parent) : QObject(parent)
{
    this->parserPool.clear();
    this->tInfo = nullptr;
}

Worker::~Worker()
{
    QHash<QString, Parser*>::iterator i;
    for (i = this->parserPool.begin(); i != this->parserPool.end(); ++i) {
        i.value()->deleteLater();
    }
    if (this->tInfo) {
        delete this->tInfo;
    }
}

/**
 * @brief Worker::initWorker
 * @param sites
 */
void Worker::initWorker(QList<Site> sites)
{
    this->initParsers(sites);
    if (!this->tInfo) {
        this->tInfo = new TInfo;
    }
    this->tInfo->clear();
    emit iAmReady();
}

/**
 * @brief Worker::initParsers
 * @param sites
 */
void Worker::initParsers(QList<Site> sites)
{
    foreach(Site site, sites) {
        Parser *parser;
        if (site.url == "https://www.autoscaners.ru/") {
            parser = new AutoscanersruParser;
        } else if (site.url == "https://www.rustehnika.ru/") {
            parser = new RustehnikaRuParser;
        } else if(site.url == "https://mactak-m.ru/") {
            parser = new Mactacmru;
        } else if (site.url == "http://pf-tool.ru/") {
            parser = new PfToolru;
        } else if (site.url == "http://www.freemen.su/") {
            parser = new FreeMenSu;
        } else if (site.url == "https://sl33.ru/") {
            parser = new Sl33;
        } else {
            parser = nullptr;
        }
        parser->setObjectName(this->objectName());
        QObject::connect(parser, &Parser::addTableStatus, this, &Worker::addTableStatus);
        this->parserPool.insert(site.url, parser);
    }
}

/**
 * @brief Worker::process
 */
void Worker::process()
{
    foreach (Site site, this->tInfo->article.sites) {
        Parser *parser = this->getParser(site.url);
        parser->process(this->tInfo->article);
    }

    emit taskDone();
}

/**
 * Фабрика парсеров
 * @brief Worker::createParser
 * @param site
 * @return
 */
Parser* Worker::getParser(QString const site)
{
    return this->parserPool.value(site);
}
