#include "flipp.h"

Flipp::Flipp(QWidget *parent)
    : QMainWindow(parent)
{
    connections = new ConnectionListWidget(this);
    terminals = new TerminalListWidget(this);
    parsers = new ParserListWidget(this);
    plotter = new PlotterWidget(this);

    connect(connections,SIGNAL(connectionListChanged(QStringList)),terminals,SLOT(updateConnections(QStringList)));
    connect(connections,SIGNAL(connectionListChanged(QStringList)),plotter,SLOT(updateConnections(QStringList)));
    connect(terminals,SIGNAL(terminalRequest(TerminalWidget*,QString)),this,SLOT(handleTerminalRequest(TerminalWidget*,QString)));
    connect(parsers,SIGNAL(parserListChanged(QStringList*)),plotter,SLOT(updateParsers(QStringList*)));
    connect(plotter,SIGNAL(plotterConnectionRequest(QString)),this,SLOT(handlePlotterConnectionRequest(QString)));
    connect(plotter,SIGNAL(plotterParserRequest(QString)),this,SLOT(handlePlotterParserRequest(QString)));

    connections->newConnection();
    terminals->newTerminal();
    parsers->newParser();
    terminals->terminalList.at(0)->assignConnection(connections->connectionList.at(0));

    setCentralWidget(plotter);

    createDocks();
    createMenus();

    this->setWindowTitle(tr("f l i p p"));

    //     this->setWindowFlags(Qt::CustomizeWindowHint); //Set window with no title bar
    //this->setWindowFlags(Qt::FramelessWindowHint); //Set a frameless window

//    QFile qss(":/styles/flipp.css");
    QFile qss("../flipp/styles/flipp.css");
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

void Flipp::handlePlotterConnectionRequest(QString name)
{
    foreach(ConnectionWidget *connection,connections->connectionList)
    {
        if(connection->getName()==name)
        {
            plotter->assignConnection(connection);
            qDebug() << tr("Connection assigned");
        }
    }
}

void Flipp::handlePlotterParserRequest(QString name)
{
    foreach(ParserWidget *parser,*parsers->parserList)
    {
        if(parser->getName()==name)
        {
            plotter->assignParser(parser);
            qDebug() << tr("Parser assigned");
        }
    }
}

void Flipp::createDocks()
{
    connectionDock = new QDockWidget(tr("Connection list"));
    connectionDock->setWidget(connections);
    connectionDock->setFeatures(QDockWidget::DockWidgetClosable|
                                QDockWidget::DockWidgetMovable|
                                QDockWidget::DockWidgetFloatable);
    connectionDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea,connectionDock);

    terminalDock = new QDockWidget(tr("Terminal list"));
    terminalDock->setWidget(terminals);
    terminalDock->setFeatures(QDockWidget::DockWidgetClosable|
                                QDockWidget::DockWidgetMovable|
                                QDockWidget::DockWidgetFloatable);
    terminalDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea,terminalDock);

    parserDock = new QDockWidget(tr("Parser list"));
    parserDock->setWidget(parsers);
    terminalDock->setFeatures(QDockWidget::DockWidgetClosable|
                                QDockWidget::DockWidgetMovable|
                                QDockWidget::DockWidgetFloatable);
    terminalDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea,parserDock);

        setTabPosition(Qt::AllDockWidgetAreas,QTabWidget::North);
}

void Flipp::createMenus()
{

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    exitAct = new QAction(tr("E&xit"), this);
         exitAct->setStatusTip(tr("Exit the application"));
         connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu->addAction(exitAct);
    QMenu *viewMenu = menuBar()->addMenu(tr("View"));
    viewMenu->addAction(connectionDock->toggleViewAction());
    viewMenu->addAction(terminalDock->toggleViewAction());
    viewMenu->addAction(parserDock->toggleViewAction());
//  viewMenu->addAction(plotsetsDock->toggleViewAction());
}
