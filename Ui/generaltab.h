#ifndef GENERALTAB_H
#define GENERALTAB_H

#include <QWidget>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QProgressBar>
#include <QSpinBox>
#include <QAction>
#include <QFileDialog>
#include <QTableWidget>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QSizePolicy>

struct Counters {
    QLabel *total;
    QLabel *done;
    QLabel *notFound;
    QLabel *errors;
    QSpinBox *threadCount;
    QProgressBar *progressBar;
};

struct Buttons {
    QPushButton *start;
    QPushButton *stop;
};

class GeneralTab : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralTab(QWidget *parent = nullptr);
    QAction *actUpload;
    QString sourceFilePath;
    Counters counters;
    Buttons buttons;
    QTableWidget *statusTable;
    QPlainTextEdit *statusText;
signals:

protected:
    QGroupBox *createUpload();
    QLineEdit *filePath;
    QGroupBox *createCounters();
    QProgressBar *createProgressBar();
    int readCountLinesInFile(QString filePath);
    QHBoxLayout *createActionButtons();
    QGroupBox *createThreadCount();
    QVBoxLayout *createTableStatus();
protected slots:
    void onActUpload();
};

#endif // GENERALTAB_H
