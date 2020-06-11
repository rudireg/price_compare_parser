#ifndef OPTIONSTAB_H
#define OPTIONSTAB_H

#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>

struct DomainToParse {
    QString url;
    QCheckBox* checkbox;
    QString title;
    DomainToParse() {
        this->url.clear();
        this->checkbox = nullptr;
        this->title.clear();
    }
};

class OptionsTab : public QWidget
{
    Q_OBJECT
public:
    explicit OptionsTab(QWidget *parent = nullptr);
    QList<DomainToParse*> domains;
    QLineEdit *captchaClientKey;

public slots:
    void selectDeselectSites();
signals:

protected:
    QCheckBox *selector;
    QWidget *createDomainOptions();
    void initDomainList();
    QLayout *createCaptchaOption();
    QWidget *createSelectDeselectCheckboxOption();
};

#endif // OPTIONSTAB_H
