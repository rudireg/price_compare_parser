#ifndef MASTER_H
#define MASTER_H

#include <QObject>
#include <QThread>
#include <QMessageBox>
#include <QPointer>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "worker.h"
#include "Network/rhttp.h"

struct Result {
    QList<Article> articles;
    void clear() {
        this->articles.clear();
    }
};

class Master : public QObject
{
    Q_OBJECT
public:
    explicit Master(QObject *parent = nullptr);
    ~Master();
protected:
    RHttp *http;
    ValuteRate *valuteRate;
    QList<QPointer<Worker>> workers;
    QList<Article> *articles;
    Result result;
    void createReport();
    void saveParserDataAsHtml();
    void processCounters(const Article article);
    void saveParserDataAsCSV();
    bool initValuteRate();
public slots:
    void startWork(QList<Article> *articles, int threadCount, QString captchaClientKey);
    void stopWork();
    void setTask(Worker *worker = nullptr);
    void workerDestroyed();
    void taskDone();
signals:
    void initWorkers(QList<Site>);
    void masterDone();
    void increaseDone();
    void increaseNotFound();
    void increaseError();
    void increasePriceNotFound();
    void increaseProgressBar();
    void addLog(QString);
    void addTableStatus(int idThread, QString article, QString status, QString color);
};

#endif // MASTER_H
