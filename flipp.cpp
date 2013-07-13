#include "flipp.h"

Flipp::Flipp(QWidget *parent)
    : QMainWindow(parent)
{
    connections = new ConnectionListWidget(this);
    terminals = new TerminalListWidget(this);
    parsers = new ParserListWidget(this);
    plotters = new PlotterListWidget(this);

    m_sSettingsFile = QApplication::applicationDirPath() + "/lastSettings.flp";
    //    qDebug() << m_sSettingsFile;
    //    initSettings();


    connect(connections,SIGNAL(connectionListChanged(QStringList)),terminals,SLOT(updateConnections(QStringList)));
    connect(connections,SIGNAL(connectionListChanged(QStringList)),plotters,SLOT(updateConnections(QStringList)));
    connect(terminals,SIGNAL(terminalRequest(TerminalWidget*,QString)),this,SLOT(handleTerminalRequest(TerminalWidget*,QString)));
    connect(parsers,SIGNAL(parserListChanged(QStringList*)),plotters,SLOT(updateParsers(QStringList*)));
    connect(plotters,SIGNAL(plotterConnectionRequest(PlotterWidget*,QString)),this,SLOT(handlePlotterConnectionRequest(PlotterWidget*,QString)));
    connect(plotters,SIGNAL(plotterParserRequest(PlotterWidget*,QString)),this,SLOT(handlePlotterParserRequest(PlotterWidget*,QString)));

    restoreSettings();

    //    plotters->newPlotter();
    setCentralWidget(plotters);

    createDocks();
    createMenus();

    this->setWindowTitle(tr("f l i p p"));

    //    QFile qss(":/styles/flipp.css");
    QFile qss("../flipp/styles/flipp.css");
    qss.open(QFile::ReadOnly);
    setStyleSheet(qss.readAll());
    qss.close();

    //    emit exit(0);
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

void Flipp::handlePlotterConnectionRequest(PlotterWidget *plotter,QString name)
{
    foreach(ConnectionWidget *connection,connections->connectionList)
    {
        if(connection->getName()==name)
        {
            plotter->assignConnection(connection);
            //            qDebug() << tr("Connection assigned");
        }
    }
}

void Flipp::handlePlotterParserRequest(PlotterWidget *plotter,QString name)
{
    foreach(ParserWidget *parser,*parsers->parserList)
    {
        if(parser->getName()==name)
        {
            plotter->assignParser(parser);
            //            qDebug() << tr("Parser assigned");
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
    //    addDockWidget(Qt::LeftDockWidgetArea,parserDock);
    addDockWidget(Qt::TopDockWidgetArea,parserDock);

    tabifyDockWidget(connectionDock,terminalDock);
    tabifyDockWidget(terminalDock,parserDock);

    setTabPosition(Qt::AllDockWidgetAreas,QTabWidget::North);
}

void Flipp::createMenus()
{

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));

    saveAct = new QAction(tr("&Save"),this);
    saveAct->setStatusTip(tr("Save current settings"));
    connect(saveAct,SIGNAL(triggered()),this,SLOT(saveSettings()));
    fileMenu->addAction(saveAct);

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

void Flipp::initSettings()
{
    //    saveState(0);
    // Set initial settings
    QSettings settings(m_sSettingsFile,QSettings::IniFormat);
    settings.clear();
    // CONNECTIONS
    settings.beginGroup("Connections");

    settings.beginGroup("GPS & Time");
    settings.setValue("Type",TCP);
    settings.setValue("Address","127.0.0.1");
    settings.setValue("Port","50500");
    settings.endGroup();

    settings.beginGroup("Dummy serial");
    settings.setValue("Type",SERIAL);
    settings.setValue("Address","COM2");
    settings.setValue("Port","115200");
    settings.endGroup();

    settings.endGroup();

    // TERMINALS

    settings.beginGroup("Terminals");

    settings.beginGroup("01");
    settings.setValue("Connection","GPS & time");
    settings.setValue("Echo",true);
    settings.endGroup();

    settings.endGroup();

    // PARSERS

    settings.beginGroup("Parsers");

    // VARIABLE WIDGETS
    settings.beginGroup("GPS");

    settings.beginGroup("GPS start delimiter");
    settings.setValue("Type",BYTTYPE);
    settings.setValue("Fixed",false);
    settings.setValue("Matched",true);
    settings.setValue("Match bytes","GPGGA,N|");
    settings.endGroup();

    settings.beginGroup("GPS Latitude");
    settings.setValue("Type",NUMTYPE);
    settings.setValue("Fixed",false);
    settings.endGroup();

    settings.endGroup();

    settings.endGroup();

    settings.sync();
}

void Flipp::restoreSettings()
{
    // Read settings file
    if(m_sSettingsFile.isEmpty())
    {
        initSettings();
        return;
    }

    QSettings settings(m_sSettingsFile,QSettings::IniFormat);

    QStringList connectionNames;
    QStringList terminalNames;
    QStringList parserNames;
    //    QStringList *parserNames = new QStringList;

    QString val;

    // Restore connection widgets
    settings.beginGroup("Connections");
    connectionNames = settings.childGroups();
    foreach(QString connectionName,settings.childGroups())
    {
        int connType;
        QString address;
        QString port;

        val.clear();
        val.append(connectionName);
        val.append("/Type");
        connType = settings.value(val).toString().toInt();

        val.clear();
        val.append(connectionName);
        val.append("/Address");
        address.append(settings.value(val).toString());

        val.clear();
        val.append(connectionName);
        val.append("/Port");
        port.append(settings.value(val).toString());

        connections->addConnection(connectionName,connType,address,port);
    }
    settings.endGroup();

    // Restore terminal widgets
    settings.beginGroup("Terminals");
    terminalNames = settings.childGroups();
    foreach(QString terminalName,terminalNames)
    {
        TerminalWidget *tw = new TerminalWidget;
        tw->updateConnections(connectionNames);
        tw->setViews(BOTH_VIEWS);
        tw->setEcho(true);
        tw->setPacketHexFormat(true);
        // Set connection

        // Add terminal widget to terminal list
        terminals->addTerminal(tw);
    }
    settings.endGroup();


    // Restore parser widgets
    settings.beginGroup("Parsers");
    parserNames = settings.childGroups();
    foreach(QString parserName,parserNames)
    {
        ParserWidget *pw = new ParserWidget;
        pw->setName(parserName);
        parserNames.append(pw->getName());
        settings.beginGroup(parserName);
        QStringList complexVariables = settings.childGroups();
        // Restore variable widgets
        foreach(QString complexVariable,complexVariables)
        {
            VariableWidget *vw = new VariableWidget;
            vw->setName(complexVariable);
            settings.beginGroup(complexVariable);
            vw->setType(settings.value("Type").toInt());
            vw->setFixed(settings.value("Fixed").toBool());
            vw->setMatched(settings.value("Matched").toBool());
            settings.endGroup();
            pw->addVariableWidget(vw);
        }

        settings.endGroup();
        parsers->addParser(pw);
    }
    settings.endGroup();

    //    ParserWidget *paw = new ParserWidget;
    //    paw->setName("Time");
    //    parserNames->append(paw->getName());

    //    // Add variables
    //    VariableWidget *varw01 = new VariableWidget;
    //    varw01->setName("Hours");
    //    varw01->setType(NUMTYPE);
    //    varw01->setFixed(true);
    //    varw01->setLength(2);

    //    VariableWidget *varw02 = new VariableWidget;
    //    varw02->setName("First colon");
    //    varw02->setType(BYTTYPE);
    //    varw02->setMatched(true);
    //    varw02->setMatchBytes(":");

    //    VariableWidget *varw03 = new VariableWidget;
    //    varw03->setName("Minutes");
    //    varw03->setType(NUMTYPE);
    //    varw03->setFixed(true);
    //    varw03->setLength(2);

    //    VariableWidget *varw04 = new VariableWidget;
    //    varw04->setName("Second colon");
    //    varw04->setType(BYTTYPE);
    //    varw04->setMatched(true);
    //    varw04->setMatchBytes(":");

    //    VariableWidget *varw05 = new VariableWidget;
    //    varw05->setName("Seconds");
    //    varw05->setType(NUMTYPE);
    //    varw05->setFixed(false);

    //    VariableWidget *varw06 = new VariableWidget;
    //    varw06->setName("End byte");
    //    varw06->setType(BYTTYPE);
    //    varw06->setFixed(true);
    //    varw06->setLength(1);

    //    paw->addVariableWidget(varw01);
    //    paw->addVariableWidget(varw02);
    //    paw->addVariableWidget(varw03);
    //    paw->addVariableWidget(varw04);
    //    paw->addVariableWidget(varw05);
    //    paw->addVariableWidget(varw06);

    //    ParserWidget *paw02 = new ParserWidget;
    //    paw02->setName("GPS");
    //    parserNames->append(paw02->getName());

    //    VariableWidget *varw11 = new VariableWidget;
    //    varw11->setName("GPS start");
    //    varw11->setType(BYTTYPE);
    //    varw11->setMatched(true);
    //    varw11->setMatchBytes("GPGGA,N|");

    //    VariableWidget *varw12 = new VariableWidget;
    //    varw12->setName("Coords");
    //    varw12->setType(NUMTYPE);
    //    varw12->setFixed(false);

    //    VariableWidget *varw13 = new VariableWidget;
    //    varw13->setName("delimiter");
    //    varw13->setType(BYTTYPE);
    //    varw13->setMatched(true);
    //    varw13->setMatchBytes("|");

    //    VariableWidget *varw14 = new VariableWidget;
    //    varw14->setName("Coords");
    //    varw14->setType(NUMTYPE);
    //    varw14->setFixed(false);

    //    VariableWidget *varw15 = new VariableWidget;
    //    varw15->setName("delimiter");
    //    varw15->setType(BYTTYPE);
    //    varw15->setMatched(true);
    //    varw15->setMatchBytes("|");

    //    VariableWidget *varw16 = new VariableWidget;
    //    varw16->setName("Coords");
    //    varw16->setType(NUMTYPE);
    //    varw16->setFixed(false);

    //    VariableWidget *varw17 = new VariableWidget;
    //    varw17->setName("end delimiter");
    //    varw17->setType(BYTTYPE);
    //    varw17->setFixed(true);
    //    varw17->setLength(1);

    //    paw02->addVariableWidget(varw11);
    //    paw02->addVariableWidget(varw12);
    //    paw02->addVariableWidget(varw13);
    //    paw02->addVariableWidget(varw14);
    //    paw02->addVariableWidget(varw15);
    //    paw02->addVariableWidget(varw16);
    //    paw02->addVariableWidget(varw17);


    // View

    // Build parser list
    //    parsers->addParser(paw);
    //    parsers->addParser(paw02);

}

void Flipp::saveSettings()
{
    // Read settings file
    QSettings settings(m_sSettingsFile,QSettings::IniFormat);
    settings.clear();

    // Connections
    settings.beginGroup("Connections");

    foreach(ConnectionWidget *connection,connections->connectionList)
    {
        settings.beginGroup(connection->getName());
        quint8 vartype = connection->getType();
        settings.setValue("Type",vartype);

        if(vartype==SERIAL)
        {
            settings.setValue("Address",connection->getSerialPort());
            settings.setValue("Port",connection->getBaudRate());
        }
        else
        {
            settings.setValue("Address",connection->getIPAddress());
            settings.setValue("Port",connection->getIPPort());
        }
        settings.endGroup();

    }
    settings.endGroup();

    // Terminals
//    settings.beginGroup("Terminals");

//    foreach(TerminalWidget *terminal,terminals->terminalList)
//    {
//        settings.beginGroup(terminal->getName);
//    }

//    settings.endGroup();



    settings.sync();
}
