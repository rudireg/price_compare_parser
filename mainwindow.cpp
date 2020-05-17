#include "mainwindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QTabWidget>
#include <QLabel>
#include <QGridLayout>

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->sitesToParse.append("https://www.autoscaners.ru/");
    this->sitesToParse.append("https://www.rustehnika.ru/");
    this->sitesToParse.append("https://mactak-m.ru/");
    this->sitesToParse.append("http://pf-tool.ru/");
    this->sitesToParse.append("http://www.freemen.su/");
    this->sitesToParse.append("https://sl33.ru/");
    this->sitesToParse.append("https://special-tool.ru/");

    createActions();
    createMenu();
    createUi();

    this->articles = new QList<Article>;
    this->master = new Master;
    this->thread = new QThread;
    this->master->moveToThread(this->thread);
    this->thread->start();

    QObject::connect(this->generalTab->buttons.start, &QPushButton::clicked, this, [this](){this->prepareData(this->generalTab->sourceFilePath);});
    QObject::connect(this->generalTab->buttons.stop, &QPushButton::clicked, this->master, &Master::stopWork);
    QObject::connect(this, &MainWindow::startWork, this->master, &Master::startWork);
    QObject::connect(this->master, &Master::masterDone, this, &MainWindow::masterDone);
    QObject::connect(this->master, &Master::increaseDone, this, &MainWindow::increaseDone);
    QObject::connect(this->master, &Master::increaseNotFound, this, &MainWindow::increaseNotFound);
    QObject::connect(this->master, &Master::increaseError, this, &MainWindow::increaseError);
    QObject::connect(this->master, &Master::increaseProgressBar, this, &MainWindow::increaseProgressBar);
    QObject::connect(this->master, &Master::addLog, this, &MainWindow::addLog);
    QObject::connect(this->master, &Master::addTableStatus, this, &MainWindow::addTableStatus);

    //Инициализировать настройки
    this->readSettings();
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    //Сохранить настройки
    this->writeSettings();
}

/**
 * @brief MainWindow::writeSettings
 */
void MainWindow::writeSettings()
{
    m_settings.beginGroup("/Settings");
    m_settings.setValue("/threadCount", this->generalTab->counters.threadCount->value());
    m_settings.endGroup();
}

/**
 * @brief MainWindow::readSettings
 */
void MainWindow::readSettings()
{
    m_settings.beginGroup("/Settings");
    //Read
    int threadCount = m_settings.value("/threadCount", 0).toInt();
    //Set
    this->generalTab->counters.threadCount->setValue(threadCount);
    m_settings.endGroup();
}

/**
 * Подготовка данных для парсера
 * @brief MainWindow::prepareData
 * @param filePath
 */
void MainWindow::prepareData(QString filePath)
{
    this->generalTab->buttons.start->setEnabled(false);
    this->generalTab->buttons.stop->setEnabled(true);
    this->articles->clear();
    this->clearCounters();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Error", "Не возможно открыть файл.");
        return;
    }

    while (!file.atEnd()) {
        QString article = file.readLine().trimmed();
        if (article.length() > 0) {
            Article data;
            data.article = article;
            foreach (QString needParse, this->sitesToParse) {
                Site site;
                site.url = needParse;
                data.sites.append(site);
            }
            this->articles->append(data);
        }
    }
    file.close();

    if (this->articles->length() < 1) {
        QMessageBox::critical(nullptr, "Error", "Нет атикулов для парсинга.");
        return;
    }

    this->generalTab->counters.progressBar->setMaximum(this->articles->count());
    int threadCount = this->generalTab->counters.threadCount->value();
    this->initStatusTable(threadCount);
    emit startWork(this->articles, threadCount);
}

/**
 * @brief MainWindow::masterDone
 */
void MainWindow::masterDone()
{
    QMessageBox::information(nullptr, "Info", "Все потоки оставновлены.\r\nРезультаты смотреть в папке Data\r\nФайлы: CSV_result.csv, HTML_result.html");
    this->enableStartButton();
}

/**
 * @brief MainWindow::enableStartButton
 */
void MainWindow::enableStartButton()
{
    this->generalTab->buttons.start->setEnabled(true);
    this->generalTab->buttons.stop->setEnabled(false);
}

/**
 * @brief MainWindow::createUi
 */
void MainWindow::createUi()
{
    QTabWidget *tabWidget = new QTabWidget();
    this->generalTab = new GeneralTab;
    this->optionsTab = new OptionsTab;
    tabWidget->addTab(this->generalTab, tr("General"));
    tabWidget->addTab(this->optionsTab, tr("Options"));

    setCentralWidget(tabWidget);
    setWindowTitle(tr("Price Parser v 1.0.1"));
}

/**
 * @brief MainWindow::createActions
 */
void MainWindow::createActions()
{
    this->actExit = new QAction(tr("&Exit"), this);
    QObject::connect(this->actExit, &QAction::triggered, this, &MainWindow::onActExit);
}

/**
 * @brief MainWindow::createManu
 */
void MainWindow::createMenu()
{
    QMenuBar *menuBar = new QMenuBar;
    QMenu *menuFile = menuBar->addMenu(tr("&File"));
    menuFile->addAction(this->actExit);
    setMenuBar(menuBar);
}

/**
 * @brief MainWindow::onAddExit
 */
void MainWindow::onActExit()
{
    //Сохранить настройки
    this->writeSettings();
    exit(0);
}

/**
 * @brief MainWindow::increaseError
 */
void MainWindow::increaseError()
{
    int count = this->generalTab->counters.errors->text().toInt();
    count ++;
    this->generalTab->counters.errors->setNum(count);
}

/**
 * @brief MainWindow::increaseDone
 */
void MainWindow::increaseDone()
{
    int count = this->generalTab->counters.done->text().toInt();
    count ++;
    this->generalTab->counters.done->setNum(count);
}

/**
 * @brief MainWindow::increaseNotFound
 */
void MainWindow::increaseNotFound()
{
    int count = this->generalTab->counters.notFound->text().toInt();
    count ++;
    this->generalTab->counters.notFound->setNum(count);
}

/**
 * @brief MainWindow::increaseProgressBar
 */
void MainWindow::increaseProgressBar()
{
    int count = this->generalTab->counters.progressBar->value();
    count ++;
    this->generalTab->counters.progressBar->setValue(count);
}

/**
 * @brief MainWindow::clearCounters
 */
void MainWindow::clearCounters()
{
    this->generalTab->counters.errors->setNum(0);
    this->generalTab->counters.notFound->setNum(0);
    this->generalTab->counters.done->setNum(0);
    this->generalTab->counters.progressBar->setValue(0);
    this->generalTab->statusText->clear();
    this->generalTab->statusTable->clear();
}

/**
 * @brief MainWindow::addLog
 * @param msg
 */
void MainWindow::addLog(QString const msg)
{
    this->generalTab->statusText->appendPlainText(msg);
}

/**
 * Отображает текущее состояние потока
 * @brief MainWindow::addStatusToTableWidget
 * @param idThread
 * @param article
 * @param status
 * @param color
 */
void MainWindow::addTableStatus(int idThread, QString article, QString status, QString color)
{
    this->generalTab->statusTable->item(idThread, 0)->setText(article);
    this->generalTab->statusTable->item(idThread, 1)->setText(status);

    QRadialGradient gradient(0, 50, 100, 50, 100);
    if (color == "red")         gradient.setColorAt(0, QColor::fromRgbF(1, 0, 0, 1));
    else if (color == "green")  gradient.setColorAt(0, QColor::fromRgbF(0, 1, 0, 1));
    else if (color == "blue")   gradient.setColorAt(0, QColor::fromRgbF(0, 0, 1, 1));
    else if (color == "orange") gradient.setColorAt(0, QColor::fromRgbF(1, 1, 0, 1));
    else                        gradient.setColorAt(0, QColor::fromRgbF(0, 1, 1, 1));

    gradient.setColorAt(1, QColor::fromRgbF(0, 0, 0, 0));
    QBrush brush(gradient);
    this->generalTab->statusTable->item(idThread, 1)->setBackground(brush);
}

/**
 * @brief MainWindow::initStatusTable
 * @param cntThr
 */
void MainWindow::initStatusTable(int cntThr)
{
    this->generalTab->statusTable->clear();
    this->generalTab->statusTable->setColumnCount(2);
    this->generalTab->statusTable->horizontalHeader()->setStretchLastSection(true);
    this->generalTab->statusTable->setRowCount(cntThr);
    QTableWidgetItem *valuesHeaderItem = new QTableWidgetItem(QString::fromUtf8("Артикул"));
    this->generalTab->statusTable->setHorizontalHeaderItem(0, valuesHeaderItem);
    valuesHeaderItem = new QTableWidgetItem(QString::fromUtf8("Статус"));
    this->generalTab->statusTable->setHorizontalHeaderItem(1, valuesHeaderItem);
    this->generalTab->statusTable->resizeColumnsToContents();

    QTableWidgetItem *newItem;
    for (int i = 0; i < cntThr; ++i) {
        this->generalTab->statusTable->setRowHeight(i, 20);
        newItem = new QTableWidgetItem("-");
        this->generalTab->statusTable->setItem(i,0,newItem);
        newItem = new QTableWidgetItem("-");
        this->generalTab->statusTable->setItem(i,1,newItem);
    }
    this->generalTab->statusTable->setColumnWidth(0, 90);
}
