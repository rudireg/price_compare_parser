#include "generaltab.h"

/**
 * @brief GeneralTab::GeneralTab
 * @param parent
 */
GeneralTab::GeneralTab(QWidget *parent) : QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(this->createUpload(), 0, 0, Qt::AlignTop);
    mainLayout->addWidget(this->createCounters(), 1, 0, Qt::AlignTop);
    mainLayout->addWidget(this->createProgressBar(), 2, 0, Qt::AlignTop);
    mainLayout->addWidget(this->createThreadCount(), 3, 0, Qt::AlignTop);
    mainLayout->addLayout(this->createActionButtons(), 4, 0, Qt::AlignTop);

    mainLayout->addLayout(this->createTableStatus(), 0, 1, 5, 1, Qt::AlignTop);

    mainLayout->setColumnStretch(0, 0);
    mainLayout->setColumnStretch(1, 1); // колонка в ширину
    mainLayout->setRowStretch(0, 0); // ряд в высоту
    mainLayout->setRowStretch(4, 1);
    setLayout(mainLayout);
}

/**
 * @brief GeneralTab::createTableStatus
 * @return
 */
QVBoxLayout *GeneralTab::createTableStatus()
{
    QVBoxLayout *layout = new QVBoxLayout;
    this->statusTable = new QTableWidget;
    this->statusText = new QPlainTextEdit;
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->addTab(this->statusTable, tr("Потоки"));
    tabWidget->addTab(this->statusText, tr("Логи"));
    layout->addWidget(tabWidget);
    return layout;
}

/**
 * @brief GeneralTab::createActionButtons
 * @return
 */
QHBoxLayout* GeneralTab::createActionButtons()
{
    QHBoxLayout *layout = new QHBoxLayout;
    this->buttons.start = new QPushButton(tr("Start"));
    this->buttons.stop = new QPushButton(tr("Stop"));
    this->buttons.start->setEnabled(false);
    this->buttons.stop->setEnabled(false);

    layout->addWidget(this->buttons.start);
    layout->addWidget(this->buttons.stop);
    return layout;
}

/**
 * @brief GeneralTab::createProgressBar
 * @return
 */
QProgressBar* GeneralTab::createProgressBar()
{
    this->counters.progressBar = new QProgressBar;
    this->counters.progressBar->setValue(0);
    return this->counters.progressBar;
}

/**
 * @brief GeneralTab::createCounters
 * @return
 */
QGroupBox* GeneralTab::createCounters()
{
    this->counters.total = new QLabel("0");
    this->counters.done = new QLabel("0");
    this->counters.notFound = new QLabel("0");
    this->counters.priceNotFound = new QLabel("0");
    this->counters.errors = new QLabel("0");

    QGroupBox *box = new QGroupBox(tr("Счетчики"));
    QFormLayout *form = new QFormLayout;
    form->addRow(tr("Всего"), this->counters.total);
    form->addRow(tr("Готово"), this->counters.done);
    form->addRow(tr("Артикуль не найден"), this->counters.notFound);
    form->addRow(tr("Цена не найдена"), this->counters.priceNotFound);
    form->addRow(tr("Ошибки"), this->counters.errors);
    box->setLayout(form);
    return box;
}

/**
 * @brief GeneralTab::createThreadCount
 * @return
 */
QGroupBox* GeneralTab::createThreadCount()
{
    this->counters.threadCount = new QSpinBox;
    this->counters.threadCount->setMinimum(1);
    this->counters.threadCount->setMaximum(10);
    this->counters.threadCount->setValue(3);
    QGroupBox *box = new QGroupBox;
    QFormLayout *form = new QFormLayout;
    form->addRow(tr("Кол-во потоков"), this->counters.threadCount);
    box->setLayout(form);
    return box;
}

/**
 * @brief GeneralTab::createUpload
 * @return
 */
QGroupBox* GeneralTab::createUpload()
{
    QGroupBox *box = new QGroupBox(tr("Загрузка файла с артикулами"));
    QGridLayout *layout = new QGridLayout;
    this->filePath = new QLineEdit;
    this->filePath->setReadOnly(true);
    QPushButton *button = new QPushButton("Файл");
    QObject::connect(button, &QPushButton::clicked, this, &GeneralTab::onActUpload);

    layout->addWidget(filePath, 0, 0);
    layout->addWidget(button, 0, 1);
    box->setLayout(layout);
    return box;
}

/**
 * @brief GeneralTab::onActUpload
 */
void GeneralTab::onActUpload()
{
    QFileDialog *dialog = new QFileDialog();
    QDir dir(".");
    dir.absolutePath();
    dialog->setDirectory(dir);
    dialog->setNameFilter(tr("CSV Files (*.csv)"));
    if (dialog->exec()) {
        this->sourceFilePath = dialog->selectedFiles().first();
        this->filePath->setText(this->sourceFilePath);
        int count = this->readCountLinesInFile(this->sourceFilePath);
        this->counters.total->setNum(count);
        this->buttons.start->setEnabled(count > 0);
    }
}

/**
 * @brief GeneralTab::readCountLinesInFile
 * @param filePath
 * @return
 */
int GeneralTab::readCountLinesInFile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return 0;
    int count = 0;
    while (!file.atEnd()) {
        if (file.readLine().trimmed().length() > 0) {
             count ++;
        }
    }
    file.close();
    return count;
}













