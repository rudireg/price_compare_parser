#ifndef OPTIONSTAB_H
#define OPTIONSTAB_H

#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>

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

signals:

protected:
    QWidget *createDomainOptions();
    void initDomainList();
};

#endif // OPTIONSTAB_H
