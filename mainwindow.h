#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QSettings>
#include <QHeaderView>
#include <QApplication>
#include "master.h"
#include "Ui/generaltab.h"
#include "Ui/optionstab.h"
#include "master.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QList<Article> *articles;
    QList<QString> sitesToParse;
public slots:
    void prepareData(QString filePath);
    void enableStartButton();
    void masterDone();
    void increaseError();
    void increaseDone();
    void increaseNotFound();
    void increaseProgressBar();
    void addLog(const QString msg);
    void addTableStatus(int idThread, QString article, QString status, QString color);
    void increasePriceNotFound();
protected:
    QSettings m_settings;
    QAction *actExit;
    Master *master;
    QThread *thread;
    GeneralTab *generalTab;
    OptionsTab *optionsTab;
    void writeSettings();
    void readSettings();
    void createUi();
    void createMenu();
    void createActions();
    void clearCounters();
    void initStatusTable(int cntThr);
    void setDomainForParser();
protected slots:

signals:
    void startWork(QList<Article>*, int, QString);
    void close();

};
#endif // MAINWINDOW_H
