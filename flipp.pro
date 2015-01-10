#-------------------------------------------------
#
# Project created by QtCreator 2013-03-02T07:49:12
#
#-------------------------------------------------

QT       += core gui network serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = flipp
TEMPLATE = app


SOURCES += main.cpp\
        flipp.cpp \
    livelistwidget/livelistwidget.cpp \
    connection/connectionlistwidget.cpp \
    connection/connectionwidget.cpp \
    terminal/terminallistwidget.cpp \
    terminal/terminalwidget.cpp \
    terminal/qterminaledit.cpp \
    parser/parserlistwidget.cpp \
    parser/vectoritemwidget.cpp \
    parser/parserengine.cpp \
    parser/variablewidget.cpp \
    parser/parserwidget.cpp \
    plotter/plotterwidget.cpp \
    plotter/qcustomplot.cpp \
    plotter/plotterlistwidget.cpp \
    linker/linkerwidget.cpp \
    linker/linkerlistwidget.cpp \
    connection/connectionunit.cpp

HEADERS  += flipp.h \
    livelistwidget/livelistwidget.h \
    connection/connectionlistwidget.h \
    connection/connectionwidget.h \
    terminal/terminallistwidget.h \
    terminal/terminalwidget.h \
    terminal/qterminaledit.h \
    parser/parserlistwidget.h \
    parser/vectoritemwidget.h \
    parser/parserengine.h \
    parser/variablewidget.h \
    parser/parserwidget.h \
    plotter/plotterwidget.h \
    plotter/qcustomplot.h \
    plotter/plotterlistwidget.h \
    linker/linkerwidget.h \
    linker/linkerlistwidget.h \
    connection/connectionunit.h

RESOURCES += \
    resources.qrc
