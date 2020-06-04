#include "optionstab.h"

OptionsTab::OptionsTab(QWidget *parent) : QWidget(parent)
{
    // Создадим список доменов, которые покажем в настройках
    this->initDomainList();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(this->createCaptchaOption());
    mainLayout->addWidget(this->createDomainOptions());
    setLayout(mainLayout);
}

/**
 * @brief OptionsTab::createCaptchaOption
 * @return
 */
QLayout* OptionsTab::createCaptchaOption()
{
    QHBoxLayout *layout = new QHBoxLayout;
    QLabel *label = new QLabel;
    label->setText("Укажите ключ с сайта <a href=\"http://getcaptchasolution.com/qbkvw6djdb\">АнтиКапча</a> (состоит из 32 символов)");
    label->setTextFormat(Qt::RichText);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label->setOpenExternalLinks(true);
    layout->addWidget(label);
    this->captchaClientKey = new QLineEdit;
    layout->addWidget(this->captchaClientKey);
    return layout;
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

    DomainToParse * domainCaoUfa = new DomainToParse;
    domainCaoUfa->url = "https://cao-ufa.ru/";
    domainCaoUfa->title = "cao-ufa.ru";
    this->domains.append(domainCaoUfa);

    DomainToParse * domainAvtomirVologda = new DomainToParse;
    domainAvtomirVologda->url = "http://avtomir-vologda.ru/";
    domainAvtomirVologda->title = "avtomir-vologda.ru";
    this->domains.append(domainAvtomirVologda);

    DomainToParse * domainTerminalTools = new DomainToParse;
    domainTerminalTools->url = "http://terminal-tools.ru/";
    domainTerminalTools->title = "terminal-tools.ru";
    this->domains.append(domainTerminalTools);

    DomainToParse * domainCarmod = new DomainToParse;
    domainCarmod->url = "https://www.carmod.ru/";
    domainCarmod->title = "www.carmod.ru";
    this->domains.append(domainCarmod);

    DomainToParse * domainServisMax = new DomainToParse;
    domainServisMax->url = "http://www.servismax.ru/";
    domainServisMax->title = "www.servismax.ru";
    this->domains.append(domainServisMax);

    DomainToParse * domainVseOborudovanie = new DomainToParse;
    domainVseOborudovanie->url = "http://vseoborudovanie.ru/";
    domainVseOborudovanie->title = "vseoborudovanie.ru";
    this->domains.append(domainVseOborudovanie);

    DomainToParse * domainScan2 = new DomainToParse;
    domainScan2->url = "https://scan2.ru/";
    domainScan2->title = "scan2.ru";
    this->domains.append(domainScan2);

    DomainToParse * domainImperiyaavto43 = new DomainToParse;
    domainImperiyaavto43->url = "https://imperiyaavto43.ru/";
    domainImperiyaavto43->title = "imperiyaavto43.ru (Используется Google reCaptcha, укажите ключ АнтиКапчи)";
    this->domains.append(domainImperiyaavto43);

    DomainToParse * domainMasterInstrument = new DomainToParse;
    domainMasterInstrument->url = "https://www.master-instrument.ru/";
    domainMasterInstrument->title = "www.master-instrument.ru (Парсит медленно. Есть цены в USD, конвертируются по курсу ЦБ)";
    this->domains.append(domainMasterInstrument);

    DomainToParse * domainArstools = new DomainToParse;
    domainArstools->url = "https://arstools.ru/";
    domainArstools->title = "arstools.ru";
    this->domains.append(domainArstools);

//    Вот список сайтов которые нужно добавлять в первую очередь

//    https://arstools.ru/
//    https://www.servicequipment.ru/
//    http://arkudateh.ru/
//    http://glav-mech.ru/
//    https://garo24.ru/
//    http://optima-tools.ru/
//    https://mnogotools.ru/
//    https://www.maslosmenka.ru/
//    http://www.top-tul.ru/
//    https://avtomag96.ru/
//    https://odas-ekb.ru/
//    http://avtokluch-market.ru/
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
