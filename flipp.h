#ifndef FLIPP_H
#define FLIPP_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDockWidget>
#include <connection/connectionlistwidget.h>
#include <terminal/terminallistwidget.h>
#include <parser/parserlistwidget.h>

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
    // Dock Widgets:
        QDockWidget *connectionDock;
        QDockWidget *parserDock;
//        QDockWidget *terminalDock;
//    QHBoxLayout *mainLayout;
private slots:
    void handleTerminalRequest(TerminalWidget*,QString);

};

#endif // FLIPP_H
