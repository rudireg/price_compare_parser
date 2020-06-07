#include "worker.h"

Worker::Worker(QObject *parent, QString captchaClientKey, ValuteRate *valuteRate) : QObject(parent), captchaClientKey(captchaClientKey)
{
    this->parserPool.clear();
    this->tInfo = nullptr;
    this->valuteRate = valuteRate;
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
        } else if (site.url == "https://special-tool.ru/") {
            parser = new SpecialToolRu;
        } else if (site.url == "https://autocheckers.ru/") {
            parser = new AutoCheckersRu;
        } else if (site.url == "https://cao-ufa.ru/") {
            parser = new CaoUfa;
        } else if (site.url == "http://avtomir-vologda.ru/") {
            parser = new AvtomirVologda;
        } else if (site.url == "http://terminal-tools.ru/") {
            parser = new TerminalTools;
        } else if (site.url == "https://www.carmod.ru/") {
            parser = new Carmod;
        } else if (site.url == "http://www.servismax.ru/") {
            parser = new ServisMax;
        } else if (site.url == "http://vseoborudovanie.ru/") {
            parser = new VseOborudovanie;
        } else if (site.url == "https://scan2.ru/") {
            parser = new Scan2;
        } else if (site.url == "https://imperiyaavto43.ru/") {
            parser = new Imperiyaavto43(nullptr, this->captchaClientKey);
        } else if (site.url == "https://www.master-instrument.ru/") {
            parser = new MasterInstrument;
            parser->setUsdRate(this->valuteRate->usd);
        } else if (site.url == "https://arstools.ru/") {
            parser = new Arstools;
        } else if (site.url == "https://www.servicequipment.ru/") {
            parser = new ServiceQuipment;
        } else if (site.url == "http://arkudateh.ru/") {
            parser = new Arkudateh;
        } else if (site.url == "https://garo24.ru/") {
            parser = new Garo24;
        } else if (site.url == "http://optima-tools.ru/") {
            parser = new OptimaTools;
        } else if (site.url == "https://mnogotools.ru/") {
            parser = new Mnogotools;
        } else if (site.url == "https://www.maslosmenka.ru/") {
            parser = new MasloSmenka;
        } else if (site.url == "http://top-tul.ru/") {
            parser = new TopTul;
        } else if (site.url == "https://avtomag96.ru/") {
            parser = new Avtomag96;
        } else if (site.url == "https://odas-ekb.ru/") {
            parser = new OdasEkb;
        } else if (site.url == "http://avtokluch-market.ru/") {
            parser = new AvtokluchMarket;
        } else {
            parser = nullptr;
        }
        parser->setObjectName(this->objectName());
        QObject::connect(parser, &Parser::addTableStatus, this, &Worker::addTableStatus);
        QObject::connect(parser, &Parser::addLog, this, &Worker::addLog);
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
