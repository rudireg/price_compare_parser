QT       += core gui widgets concurrent network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Network/rhttp.cpp \
    Parser/autocheckersru.cpp \
    Parser/autoscanersruparser.cpp \
    Parser/avtomirvologda.cpp \
    Parser/caoufa.cpp \
    Parser/freemensu.cpp \
    Parser/mactacmru.cpp \
    Parser/parser.cpp \
    Parser/pftoolru.cpp \
    Parser/rstring.cpp \
    Parser/rustehnikaruparser.cpp \
    Parser/sl33.cpp \
    Parser/specialtoolru.cpp \
    Parser/terminaltools.cpp \
    Ui/generaltab.cpp \
    Ui/optionstab.cpp \
    main.cpp \
    mainwindow.cpp \
    master.cpp \
    worker.cpp

HEADERS += \
    Network/rhttp.h \
    Parser/autocheckersru.h \
    Parser/autoscanersruparser.h \
    Parser/avtomirvologda.h \
    Parser/caoufa.h \
    Parser/freemensu.h \
    Parser/mactacmru.h \
    Parser/parser.h \
    Parser/pftoolru.h \
    Parser/rstring.h \
    Parser/rustehnikaruparser.h \
    Parser/sl33.h \
    Parser/specialtoolru.h \
    Parser/terminaltools.h \
    Ui/generaltab.h \
    Ui/optionstab.h \
    mainwindow.h \
    master.h \
    worker.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win64 {
    LIBS += -llibssl-1_1-x64 -llibcrypto-1_1-x64
}
