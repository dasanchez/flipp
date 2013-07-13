#ifndef FLIPP_H
#define FLIPP_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QMenuBar>
#include <QSettings>
#include <connection/connectionlistwidget.h>
#include <terminal/terminallistwidget.h>
#include <parser/parserlistwidget.h>
#include <plotter/plotterlistwidget.h>

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
    PlotterListWidget *plotters;
    // Dock Widgets:
    QDockWidget *connectionDock;
    QDockWidget *parserDock;
    QDockWidget *terminalDock;
    //    QHBoxLayout *mainLayout;
    // Actions:
    QAction *exitAct;
    QAction *saveAct;

    // Settings
    //    QSettings *settings;
    QString m_sSettingsFile;

    void createDocks();
    void createMenus();
    void initSettings();
    void restoreSettings();

private slots:
    void handleTerminalRequest(TerminalWidget*,QString);
    void handlePlotterConnectionRequest(PlotterWidget*,QString);
    void handlePlotterParserRequest(PlotterWidget*,QString);
    void saveSettings();


};

#endif // FLIPP_H
