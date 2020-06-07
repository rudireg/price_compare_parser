#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QHash>
#include "Parser/parser.h"
#include "Parser/rustehnikaruparser.h"
#include "Parser/autoscanersruparser.h"
#include "Parser/mactacmru.h"
#include "Parser/pftoolru.h"
#include "Parser/freemensu.h"
#include "Parser/sl33.h"
#include "Parser/specialtoolru.h"
#include "Parser/autocheckersru.h"
#include "Parser/caoufa.h"
#include "Parser/avtomirvologda.h"
#include "Parser/terminaltools.h"
#include "Parser/carmod.h"
#include "Parser/servismax.h"
#include "Parser/vseoborudovanie.h"
#include "Parser/scan2.h"
#include "Parser/imperiyaavto43.h"
#include "Parser/masterinstrument.h"
#include "Parser/arstools.h"
#include "Parser/servicequipment.h"
#include "Parser/arkudateh.h"
#include "Parser/garo24.h"
#include "Parser/optimatools.h"
#include "Parser/mnogotools.h"
#include "Parser/maslosmenka.h"
#include "Parser/toptul.h"
#include "Parser/avtomag96.h"
#include "Parser/odasekb.h"
#include "Parser/avtokluchmarket.h"

/**
 * Структура хранит данные для рабочего потока
 * @brief The TInfo struct
 */
struct TInfo {
    int     status;
    int     task;
    int     lastError;
    QString errorText;
    Article article;
    void clear() {
//        this->status = wsNOTHING;
//        this->task   = wtNOTHING;
        this->lastError = 0;
        this->errorText.clear();
//        this->article = nullptr;
    }
};

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr, QString captchaClientKey = "", ValuteRate *valuteRate = nullptr);
    ~Worker();
    TInfo *tInfo;

public slots:
    void initWorker(QList<Site> sites);
    void process();
protected:
    ValuteRate *valuteRate;
    QHash<QString, Parser*> parserPool;
    QString captchaClientKey;
    Parser *getParser(const QString site);
    void initParsers(QList<Site> sites);
signals:
    void iAmReady();
    void taskDone();
    void addTableStatus(int idThread, QString article, QString status, QString color);
    void addLog(QString);
};

#endif // WORKER_H
