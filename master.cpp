#include "master.h"

Master::Master(QObject *parent) : QObject(parent)
{
    this->result.clear();
}

Master::~Master()
{
    QList<QPointer<Worker>>::iterator i;
    for (i = this->workers.begin(); i != this->workers.end(); ++i) {
        (*i)->deleteLater();
    }
}

/**
 * Начало работы
 * @brief Master::initMaster
 */
void Master::startWork(QList<Article> *articles, int threadCount)
{
    this->result.clear();
    this->articles = articles;

    // создаем рабочих
    for (int i = 0; i < threadCount; ++i) {
        Worker *worker = new Worker;
        worker->setObjectName(QString::number(i));
        QThread *thread = new QThread;
        QObject::connect(this, &Master::initWorkers, worker, &Worker::initWorker);
        QObject::connect(worker, SIGNAL(iAmReady()), this, SLOT(setTask()));
        QObject::connect(worker, &Worker::destroyed, this, &Master::workerDestroyed);
        QObject::connect(worker, &Worker::taskDone, this, &Master::taskDone);
        QObject::connect(worker, &Worker::addTableStatus, this, &Master::addTableStatus);
        this->workers.append(worker);
        worker->moveToThread(thread);
        thread->start();
    }

    // инициализация и запуск рабочих
    emit initWorkers(this->articles->at(0).sites);
}

void Master::stopWork()
{
}

/**
 * Назначить задание потоку
 * @brief Master::setTask
 */
void Master::setTask(Worker *worker)
{
    if (!worker) {
        worker = static_cast<Worker*>(this->sender());
    }

    // Если больше нет заданий - остановим и удалим поток
    if (!this->articles->length()) {
        QThread *thread = worker->thread();
        worker->deleteLater();
        thread->quit();
        emit addTableStatus(worker->objectName().toInt(), "", "Поток остановлен", "green");
        return;
    }

    worker->tInfo->article = this->articles->takeFirst();
    emit addTableStatus(worker->objectName().toInt(), worker->tInfo->article.article, "Начаинаем парсить", "green");
    QMetaObject::invokeMethod(worker, "process", Qt::QueuedConnection);
}

/**
 * @brief Master::taskDone
 */
void Master::taskDone()
{
    Worker *worker = static_cast<Worker*>(this->sender());
    // save parse result
    this->result.articles.append(worker->tInfo->article);
    // process counters
    emit addTableStatus(worker->objectName().toInt(), worker->tInfo->article.article, "Артикул спарсен", "green");
    this->processCounters(worker->tInfo->article);
    // take new task
    worker->tInfo->article.clear();
    this->setTask(worker);
}

/**
 * @brief Master::processCounters
 * @param article
 */
void Master::processCounters(Article const article)
{
    foreach (Site site, article.sites) {
        if (site.parserStatus == ParserStatus::ERROR_EMPTY_INBUF) {
            emit addLog(QString("[ERROR] %1:%2 Пустой ответ сайта").arg(article.article).arg(site.url));
            emit increaseError();
        } else if (site.parserStatus == ParserStatus::ERROR_PRICE_NOT_FOUND) {
            emit addLog(QString("[WARNING] %1:%2 Цена не найдена").arg(article.article).arg(site.url));
            emit increasePriceNotFound();
        } else if (site.parserStatus == ParserStatus::WARNING_ARTICLE_NOT_FOUND){
            emit increaseNotFound();
        }
        if (!site.errorMessage.isEmpty()) {
            emit addLog(QString("%1:%2: %3").arg(article.article).arg(site.url).arg(site.errorMessage));
        } else {
            emit addLog(QString("[OK] %1:%2").arg(article.article).arg(site.url));
        }
    }
    emit increaseDone();
    emit increaseProgressBar();
}

/**
 * @brief Master::workerDestroyed
 */
void Master::workerDestroyed()
{
    if (this->workers.isEmpty()) {
        return;
    }

    QMutableListIterator<QPointer<Worker>> i(this->workers);
    while (i.hasNext()) {
        if (i.next() == nullptr) {
            i.remove();
        }
    }

    if (this->workers.isEmpty()) {
        // с этого момента работа завершена, поэтому формируем отченость
        this->createReport();
        emit masterDone();
    }
}

/**
 * @brief Master::createReport
 */
void Master::createReport()
{
    this->saveParserDataAsHtml();
    this->saveParserDataAsCSV();
}

/**
 * Сохранить данные в формате html
 * @brief Master::saveHtmlParserData
 */
void Master::saveParserDataAsHtml()
{
    QString color;
    QDir dir(".");
    QString currentDir = dir.absolutePath();
    QFile filename(currentDir + "/Data/HTML_result.html");
    filename.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&filename);
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    //****************************
    out << "<html>"
           "<head><style>table {border-collapse:collapse;} table,th,td{border: 1px solid #ccc;}</style>"
           "</head>"
           "<body>"
           "<table cellspacing='2' border='1' cellpadding='5'>";

    //Заголовоки
    out << "<tr>";
    out << QString("<td>%1</td>").arg(QString::fromUtf8("Артикул"));
    foreach (Site site, this->result.articles.at(0).sites) {
        out << QString("<td>%1</td>").arg(site.url);
    }
    out << "</tr>";

    //Данные
    foreach(Article article, this->result.articles) {
        out << "<tr>";
        out << QString("<td>%1</td>").arg(article.article);
        float sourcePrice = article.sites.at(0).price;
        foreach(Site site, article.sites) {
            QString color = "";
            if (sourcePrice > site.price) {
                color = "red";
            } else if (sourcePrice < site.price) {
                color = "green";
            } else {
                color = "black";
            }
            if (site.url == "https://www.autoscaners.ru/") {
                color = "black";
            }
            QString url = site.productUrl.isEmpty()? site.searchUrl : site.productUrl;
            if (site.price < 1) {
                out << QString("<td>0 &nbsp;&nbsp;&nbsp;<a title='Поисковый запрос' style='text-decoration:none;' target='_blanc' href='%1'>&#128270;</a></td>").arg(site.searchUrl);
            } else {
                out << QString("<td><a title='Ссылка на продукт' target='_blanc' style='font-weight:600; color:%1;text-decoration:none;' href='%2'>%3</a></td>").arg(color).arg(url).arg(site.price);
            }
        };
        out << "</tr>";
    }

    out << "</table>"
           "</body>"
           "</html>";
    filename.close();
}

/**
 * @brief Master::saveParserDataAsCSV
 */
void Master::saveParserDataAsCSV()
{
    QString color;
    QDir dir(".");
    QString currentDir = dir.absolutePath();
    QFile filename(currentDir + "/Data/CSV_result.csv");
    filename.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&filename);
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    //Заголовоки
    out << QString("%1").fromUtf8("Артикул");
    foreach (Site site, this->result.articles.at(0).sites) {
        out << QString(";%1").arg(site.url);
    }
    out << QString("\r\n");

    //Данные
    foreach(Article article, this->result.articles) {
        out << QString("%1").arg(article.article);
        foreach(Site site, article.sites) {
            out << QString(";%1").arg(site.price);
        };
        out << QString("\r\n");
    }

    filename.close();
}










