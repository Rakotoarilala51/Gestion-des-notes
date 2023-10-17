#-------------------------------------------------
#
# Project created by QtCreator 2023-10-03T14:25:07
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Projet-L1
TEMPLATE = app


SOURCES += main.cpp\
        accueil.cpp \
    notes.cpp \
    bulletin.cpp

HEADERS  += accueil.h \
    notes.h \
    bulletin.h

FORMS    += accueil.ui \
    notes.ui \
    bulletin.ui

RESOURCES += \
    resources.qrc
