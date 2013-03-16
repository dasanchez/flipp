#ifndef FLIPP_H
#define FLIPP_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QMenuBar>
#include <connection/connectionlistwidget.h>
#include <terminal/terminallistwidget.h>
#include <parser/parserlistwidget.h>
#include <plotter/plotterwidget.h>

class Flipp : public QMainWindow
{
    Q_OBJECT
    
public:
    Flipp(QWidget *parent = 0);
    ~Flipp();
private:

    ConnectionListWidget *connections;
    TerminalListWidget *terminals;
    ParserListWidget *parsers;
    PlotterWidget *plotter;
    // Dock Widgets:
        QDockWidget *connectionDock;
        QDockWidget *parserDock;
        QDockWidget *terminalDock;
//    QHBoxLayout *mainLayout;
        // Actions:
        QAction *exitAct;

        void createDocks();
        void createMenus();
private slots:
    void handleTerminalRequest(TerminalWidget*,QString);
    void handlePlotterConnectionRequest(QString);
    void handlePlotterParserRequest(QString);

};

#endif // FLIPP_H
