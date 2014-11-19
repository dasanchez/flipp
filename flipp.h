#ifndef FLIPP_H
#define FLIPP_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QMenuBar>
#include <QSettings>
#include <QShortcut>
#include "connection/connectionlistwidget.h"
#include "terminal/terminallistwidget.h"
#include "parser/parserlistwidget.h"
#include "plotter/plotterlistwidget.h"
#include "linker/linkerlistwidget.h"
#include "plotter/plotterwidget.h"

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
//    PlotterListWidget *plotters;
    LinkerListWidget *linkers;
    // Dock Widgets:
    QDockWidget *connectionDock;
    QDockWidget *parserDock;
    QDockWidget *terminalDock;
    QDockWidget *linkerDock;

    //    QHBoxLayout *mainLayout;
    // Actions:
    QAction *exitAct;
    QAction *saveAct;
    QAction *dockAct;

    // Settings
    //    QSettings *settings;
    QString m_sSettingsFile;

    void createDocks();
    void createMenus();
    void initSettings();
    void restoreSettings();

private slots:
    void handleLinkerConnectionRequest(LinkerWidget*, QString);
    void handleLinkerParserRequest(LinkerWidget*,QString);
    void handleTerminalRequest(TerminalWidget*,QString);
//    void handlePlotterConnectionRequest(PlotterWidget*,QString);
//    void handlePlotterParserRequest(PlotterWidget*,QString);
    void saveSettings();
    void dockWidgets();


};

#endif // FLIPP_H
