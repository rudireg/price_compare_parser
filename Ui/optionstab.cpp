#include "optionstab.h"

OptionsTab::OptionsTab(QWidget *parent) : QWidget(parent)
{
    // Создадим список доменов, которые покажем в настройках
    this->initDomainList();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(this->createDomainOptions());
    setLayout(mainLayout);
}

/**
 * Создать домены - готовые к парсингу
 * @brief OptionsTab::initDomainList
 */
void OptionsTab::initDomainList()
{
    DomainToParse * domainAutoscaners = new DomainToParse;
    domainAutoscaners->url = "https://www.autoscaners.ru/";
    domainAutoscaners->title = "www.autoscaners.ru";
    this->domains.append(domainAutoscaners);

    DomainToParse * domainRustehnika = new DomainToParse;
    domainRustehnika->url = "https://www.rustehnika.ru/";
    domainRustehnika->title = "www.rustehnika.ru";
    this->domains.append(domainRustehnika);

    DomainToParse * domainMactakm = new DomainToParse;
    domainMactakm->url = "https://mactak-m.ru/";
    domainMactakm->title = "mactak-m.ru";
    this->domains.append(domainMactakm);

    DomainToParse * domainPftool = new DomainToParse;
    domainPftool->url = "http://pf-tool.ru/";
    domainPftool->title = "pf-tool.ru";
    this->domains.append(domainPftool);

    DomainToParse * domainFreemen = new DomainToParse;
    domainFreemen->url = "http://www.freemen.su/";
    domainFreemen->title = "www.freemen.su (Тупит на больших потоках)";
    this->domains.append(domainFreemen);

    DomainToParse * domainSl33 = new DomainToParse;
    domainSl33->url = "https://sl33.ru/";
    domainSl33->title = "sl33.ru (Если часто парсить, вылазит капча)";
    this->domains.append(domainSl33);

    DomainToParse * domainSpecialTool = new DomainToParse;
    domainSpecialTool->url = "https://special-tool.ru/";
    domainSpecialTool->title = "special-tool.ru";
    this->domains.append(domainSpecialTool);

    DomainToParse * domainAutocheckers = new DomainToParse;
    domainAutocheckers->url = "https://autocheckers.ru/";
    domainAutocheckers->title = "autocheckers.ru";
    this->domains.append(domainAutocheckers);
}

/**
 * @brief OptionsTab::createDomainOptions
 */
QWidget* OptionsTab::createDomainOptions()
{
    QGroupBox *box = new QGroupBox(tr("Домены участвуют в парсинге"));
    QVBoxLayout *layout = new QVBoxLayout;

    foreach(DomainToParse *domain, this->domains) {
        domain->checkbox = new QCheckBox(domain->title, this);
        layout->addWidget(domain->checkbox);
    }

    box->setLayout(layout);
    return box;
}
