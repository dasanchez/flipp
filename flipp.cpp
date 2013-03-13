#include "flipp.h"

Flipp::Flipp(QWidget *parent)
    : QMainWindow(parent)
{
    connections = new ConnectionListWidget(this);
    terminals = new TerminalListWidget(this);
    parsers = new ParserListWidget(this);


    connect(connections,SIGNAL(connectionListChanged(QStringList)),terminals,SLOT(updateConnections(QStringList)));
    connect(terminals,SIGNAL(terminalRequest(TerminalWidget*,QString)),this,SLOT(handleTerminalRequest(TerminalWidget*,QString)));

    connections->newConnection();
    terminals->newTerminal();
    parsers->newParser();
    parsers->newParser();
    terminals->terminalList.at(0)->assignConnection(connections->connectionList.at(0));

    connectionDock = new QDockWidget(tr("Connection list"));
    connectionDock->setWidget(connections);
    connectionDock->setFeatures(QDockWidget::DockWidgetClosable|
                                QDockWidget::DockWidgetMovable|
                                QDockWidget::DockWidgetFloatable);
    connectionDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea,connectionDock);

    parserDock = new QDockWidget(tr("Parser list"));
    parserDock->setWidget(parsers);


    setCentralWidget(terminals);

    addDockWidget(Qt::BottomDockWidgetArea,connectionDock);
    addDockWidget(Qt::BottomDockWidgetArea,parserDock);
    setTabPosition(Qt::AllDockWidgetAreas,QTabWidget::North);

    this->setWindowTitle(tr("f l i p p"));

    //     this->setWindowFlags(Qt::CustomizeWindowHint); //Set window with no title bar
    //this->setWindowFlags(Qt::FramelessWindowHint); //Set a frameless window

    QFile qss(":/styles/flipp.css");
    qss.open(QFile::ReadOnly);
    setStyleSheet(qss.readAll());
    qss.close();
}

Flipp::~Flipp()
{
    
}

void Flipp::handleTerminalRequest(TerminalWidget *terminal,QString name)
{
    foreach(ConnectionWidget *connection,connections->connectionList)
    {
        if(connection->getName()==name)
        {
            terminal->assignConnection(connection);
        }
    }
}
