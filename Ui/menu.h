#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QMenuBar>

class Menu : public QMenuBar
{
    Q_OBJECT
public:
    explicit Menu(QWidget *parent = nullptr);

signals:

};

#endif // MENU_H
