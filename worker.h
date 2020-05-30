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
    explicit Worker(QObject *parent = nullptr);
    ~Worker();
    TInfo *tInfo;

public slots:
    void initWorker(QList<Site> sites);
    void process();
protected:
    QHash<QString, Parser*> parserPool;

    Parser *getParser(const QString site);
    void initParsers(QList<Site> sites);
signals:
    void iAmReady();
    void taskDone();
    void addTableStatus(int idThread, QString article, QString status, QString color);
};

#endif // WORKER_H
