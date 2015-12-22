#-------------------------------------------------
#
# Project created by QtCreator 2015-09-03T19:39:51
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Widgets
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    RoundDial/round_dial.cpp \
    GraphBuilder/graph_builder.cpp

HEADERS  += mainwindow.h \
    RoundDial/round_dial.h \
    GraphBuilder/graph_builder.h

FORMS    += mainwindow.ui
