#include "optionstab.h"

#include <QVBoxLayout>
#include <QLabel>

OptionsTab::OptionsTab(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(new QLabel(tr("Coming soon ...")));
    setLayout(mainLayout);
}
