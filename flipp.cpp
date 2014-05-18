#include "flipp.h"

Flipp::Flipp(QWidget *parent)
    : QMainWindow(parent)
{
    connections = new ConnectionListWidget(this);
    terminals = new TerminalListWidget(this);
    parsers = new ParserListWidget(this);
    //    plotters = new PlotterListWidget(this);
    linkers = new LinkerListWidget(this);

    m_sSettingsFile = QApplication::applicationDirPath() + "/lastSettings.flp";
    //    qDebug() << m_sSettingsFile;
    //        initSettings();


    connect(connections,SIGNAL(connectionListChanged(QStringList)),terminals,SLOT(updateConnections(QStringList)));


    //    connect(connections,SIGNAL(connectionListChanged(QStringList)),plotters,SLOT(updateConnections(QStringList)));

    connect(terminals,SIGNAL(terminalRequest(TerminalWidget*,QString)),this,SLOT(handleTerminalRequest(TerminalWidget*,QString)));
    connect(connections,SIGNAL(connectionListChanged(QStringList)),linkers,SLOT(updateConnections(QStringList)));
    connect(parsers,SIGNAL(parserListChanged(QStringList)),linkers,SLOT(updateParsers(QStringList)));


    connect(linkers,SIGNAL(linkerConnectionRequest(LinkerWidget*,QString)),this,SLOT(handleLinkerConnectionRequest(LinkerWidget*, QString)));
    connect(linkers,SIGNAL(linkerParserRequest(LinkerWidget*,QString)),this,SLOT(handleLinkerParserRequest(LinkerWidget*, QString)));


    //    connect(parsers,SIGNAL(parserListChanged(QStringList)),plotters,SLOT(updateParsers(QStringList)));
    //    connect(plotters,SIGNAL(plotterConnectionRequest(PlotterWidget*,QString)),this,SLOT(handlePlotterConnectionRequest(PlotterWidget*,QString)));
    //    connect(plotters,SIGNAL(plotterParserRequest(PlotterWidget*,QString)),this,SLOT(handlePlotterParserRequest(PlotterWidget*,QString)));

    //    plotters->newPlotter();
    //    setCentralWidget(plotters);
    setCentralWidget(terminals);

    createDocks();
    createMenus();

    restoreSettings();


    this->setWindowTitle(tr("f l i p p"));


    QFile qss("../flipp/styles/flipp.css");
    qss.open(QFile::ReadOnly);
    setStyleSheet(qss.readAll());
    qss.close();

    //    emit exit(0);
}

Flipp::~Flipp()
{

}

void Flipp::handleLinkerConnectionRequest(LinkerWidget* linker, QString name)
{
    foreach(ConnectionWidget *connection,connections->connectionList)
    {
        if(connection->getName()==name)
        {

            linker->assignConnection(connection);
        }
    }
}

void Flipp::handleLinkerParserRequest(LinkerWidget* linker, QString name)
{
    foreach(ParserWidget *parser,*parsers->parserList)
    {
        if(parser->getName()==name)
        {

            linker->assignParser(parser);
        }
    }
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
        }
    }
}

void Flipp::createDocks()
{
    connectionDock = new QDockWidget(tr("Connection list"));
    connectionDock->setObjectName("Connections_Dock");
    connectionDock->setWidget(connections);
    connectionDock->setFeatures(QDockWidget::DockWidgetClosable|
                                QDockWidget::DockWidgetMovable|
                                QDockWidget::DockWidgetFloatable);
    connectionDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    addDockWidget(Qt::TopDockWidgetArea,connectionDock);

    //    terminalDock = new QDockWidget(tr("Terminal list"));
    //    terminalDock->setObjectName("Terminals_Dock");
    //    terminalDock->setWidget(terminals);
    //    terminalDock->setFeatures(QDockWidget::DockWidgetClosable|
    //                              QDockWidget::DockWidgetMovable|
    //                              QDockWidget::DockWidgetFloatable);
    //    terminalDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    //    addDockWidget(Qt::TopDockWidgetArea,terminalDock);

    parserDock = new QDockWidget(tr("Parser list"));
    parserDock->setObjectName("Parsers_Dock");
    parserDock->setWidget(parsers);
    parserDock->setFeatures(QDockWidget::DockWidgetClosable|
                            QDockWidget::DockWidgetMovable|
                            QDockWidget::DockWidgetFloatable);
    parserDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea,parserDock);

    linkerDock = new QDockWidget(tr("Linker list"));
    linkerDock->setObjectName("Linkers_Dock");
    linkerDock->setWidget(linkers);
    linkerDock->setFeatures(QDockWidget::DockWidgetClosable|
                            QDockWidget::DockWidgetMovable|
                            QDockWidget::DockWidgetFloatable);
    linkerDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea,linkerDock);

    setTabPosition(Qt::AllDockWidgetAreas,QTabWidget::North);
}

void Flipp::createMenus()
{

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));

    saveAct = new QAction(tr("&Save"),this);
    saveAct->setShortcut(QKeySequence(tr("Ctrl+s")));
    saveAct->setStatusTip(tr("Save current settings"));
    connect(saveAct,SIGNAL(triggered()),this,SLOT(saveSettings()));
    fileMenu->addAction(saveAct);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu->addAction(exitAct);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));

    dockAct = new QAction(tr("&Dock Windows"),this);
    dockAct->setShortcut(QKeySequence(tr("Ctrl+d")));
    connect(dockAct,SIGNAL(triggered()),this,SLOT(dockWidgets()));
    viewMenu->addAction(dockAct);

    connectionDock->toggleViewAction()->setShortcut(QKeySequence(tr("Alt+Q")));
    viewMenu->addAction(connectionDock->toggleViewAction());


    //    terminalDock->toggleViewAction()->setShortcut(QKeySequence(tr("Alt+W")));
    //    viewMenu->addAction(terminalDock->toggleViewAction());
    parserDock->toggleViewAction()->setShortcut(QKeySequence(tr("Alt+E")));
    viewMenu->addAction(parserDock->toggleViewAction());
    linkerDock->toggleViewAction()->setShortcut(QKeySequence(tr("Alt+L")));
    viewMenu->addAction(linkerDock->toggleViewAction());
}

void Flipp::dockWidgets()
{
    connectionDock->setFloating(false);
    //    terminalDock->setFloating(false);
    parserDock->setFloating(false);
    linkerDock->setFloating(false);
}

void Flipp::initSettings()
{
    //    saveState(0);
    // Set initial settings
    QSettings settings(m_sSettingsFile,QSettings::IniFormat);
    settings.clear();
    // CONNECTIONS
    settings.beginWriteArray("Connections");
    settings.setArrayIndex(0);
    settings.setValue("Name","GPS & Time");
    settings.setValue("Type",TCP);
    settings.setValue("Address","127.0.0.1");
    settings.setValue("Port","50500");
    settings.endArray();

    // TERMINALS

    settings.beginWriteArray("Terminals");

    settings.setArrayIndex(0);
    settings.setValue("Connection","GPS & Time");
    settings.setValue("Echo",true);

    settings.endArray();

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
    restoreSettings();
}

void Flipp::restoreSettings()
{
    // Read settings file
    QFile file(m_sSettingsFile);
    if(!file.open(QIODevice::ReadOnly))
    {
        initSettings();
        return;
    }
    else
    {
        file.close();
    }

    QSettings settings(m_sSettingsFile,QSettings::IniFormat);

    restoreState(settings.value("DOCK_LOCATIONS").toByteArray(),1);

    QStringList connectionNames;
    QStringList parserNames;

    // Restore connection widgets
    int connCount = settings.beginReadArray("Connections");
    for(int i=0;i<connCount;i++)
    {
        settings.setArrayIndex(i);
        connectionNames.append(settings.value("Name").toString());
        connections->addConnection(settings.value("Name").toString(),settings.value("Type").toInt(),settings.value("Address").toString(),settings.value("Port").toString());
    }
    settings.endArray();

    // Restore terminal widgets
    int terminalCount = settings.beginReadArray("Terminals");
    for(int i=0;i<terminalCount;i++)
    {
        settings.setArrayIndex(i);
        TerminalWidget *tw = new TerminalWidget;

        // Set views

        // Add terminal widget to terminal list
        terminals->addTerminal(tw);

        // Set connections
        tw->updateConnections(connectionNames);
        tw->changeConnection(settings.value("Connection").toString());
    }

    settings.endArray();

    // Restore parser widgets
    int parserCount = settings.beginReadArray("Parsers");

    for(int i=0;i<parserCount;i++)
    {
        settings.setArrayIndex(i);
        ParserWidget *pw = new ParserWidget;
        pw->setName(settings.value("Name").toString());
        parserNames.append(pw->getName());
        //        settings.beginGroup(parserName);
        int varSize = settings.beginReadArray("Complex");
        for(int j=0;j<varSize;j++)
        {
            settings.setArrayIndex(j);
            VariableWidget *vw = new VariableWidget;
            //            vw->itemList->clear();
            vw->setName(settings.value("Name").toString());
            vw->setType(settings.value("Type").toInt());
            if(settings.value("Type").toInt()==VECTYPE)
            {
                vw->setRepeat(settings.value("Repeat").toInt());
                int vecSize = settings.beginReadArray("Base");
                for(int k=0;k<vecSize;k++)
                {
                    settings.setArrayIndex(k);

                    // Get variable features and initialize each vector item with them:
                    BaseVariable bv;// = new BaseVariable;
                    bv.name = settings.value("Name").toString();
                    if(settings.value("Fixed").toBool())
                    {
                        bv.fixed = true;
                        bv.length = settings.value("Length").toInt();
                    }
                    else
                    {
                        bv.fixed = false;
                    }

                    if(settings.value("Type").toInt()==NUMTYPE)
                    {
                        bv.type = NUMTYPE;

                    }
                    else
                    {
                        bv.type = BYTTYPE;
                        if(settings.value("Match").toBool())
                        {
                            bv.match = true;
                            bv.matchBytes = settings.value("MBytes").toByteArray();
                        }
                    }
                    vw->addVectorVariable(bv);
                }
                settings.endArray();
            }
            else
            {
                vw->setFixed(settings.value("Fixed").toBool());
                if(vw->fixed)
                {
                    vw->setLength(settings.value("Length").toInt());
                }
                vw->setMatched(settings.value("Match").toBool());
                if(vw->matched)
                {
                    vw->setMatchBytes(settings.value("MBytes").toByteArray());
                }
            }
            pw->addVariableWidget(vw);
        }
        settings.endArray();

        //        settings.endGroup();
        parsers->addParser(pw);
    }
    settings.endArray();
    //    settings.endGroup();

    // Restore plotters
    //    int plotSize = settings.beginReadArray("Plotters");
    //    for(int i=0;i<plotSize;i++)
    //    {
    //        settings.setArrayIndex(i);
    //        PlotterWidget *pw = new PlotterWidget;

    //        pw->updateConnections(connectionNames);
    //        pw->updateParsers(parserNames);
    //        plotters->splitter->addWidget(pw);
    //        plotters->plotterList.append(pw);

    //        connect(pw,SIGNAL(plotterConnectionRequest(QString)),plotters,SLOT(plotterRequestedConnection(QString)));
    //        connect(pw,SIGNAL(plotterParserRequest(QString)),plotters,SLOT(plotterRequestedParser(QString)));
    //        connect(pw,SIGNAL(removePlotter()),plotters,SLOT(plotterRemoved()));
    //        pw->changeConnection(settings.value("Connection").toString());
    //        pw->changeParser(settings.value("Parser").toString());
    //    }
    //   settings.endArray();

}


void Flipp::saveSettings()
{
    // Read settings file
    QSettings settings(m_sSettingsFile,QSettings::IniFormat);
    settings.clear();

    // Connections
    settings.beginWriteArray("Connections");

    for(int i=0;i<connections->connectionList.size();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("Name",connections->connectionList.at(i)->getName());
        int vartype = connections->connectionList.at(i)->getType();
        settings.setValue("Type",vartype);
        if(vartype==SERIAL)
        {
            settings.setValue("Address",connections->connectionList.at(i)->getSerialPort());
            settings.setValue("Port",connections->connectionList.at(i)->getBaudRate());
        }
        else
        {
            settings.setValue("Address",connections->connectionList.at(i)->getIPAddress());
            settings.setValue("Port",connections->connectionList.at(i)->getIPPort());
        }
    }
    settings.endArray();

    // Terminals
    settings.beginWriteArray("Terminals");

    for(int i=0;i<terminals->terminalList.size();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("Connection",terminals->terminalList.at(i)->currentConnection());
    }
    settings.endArray();

    // Parsers
    settings.beginWriteArray("Parsers");
    int parserCount=0;
    foreach(ParserWidget *pw, *parsers->parserList)
    {
        settings.setArrayIndex(parserCount);
        settings.setValue("Name",pw->getName());

        // Save an array of variable widgets

        settings.beginWriteArray("Complex");
        for(int i=0;i<pw->variableList.size();i++)
        {
            settings.setArrayIndex(i);
            settings.setValue("Name",pw->variableList[i].name);
            settings.setValue("Type",pw->variableList[i].type);
            if(pw->variableList[i].type==VECTYPE)
            {
                settings.setValue("Repeat",pw->variableList[i].repeat);
                // Save an array of base variables
                settings.beginWriteArray("Base");
                for(int j=0;j<pw->variableList[i].vector.size();j++)
                {
                    settings.setArrayIndex(j);

                    settings.setValue("Name",pw->variableList[i].vector.at(j).name);
                    settings.setValue("Type",pw->variableList[i].vector.at(j).type);
                    settings.setValue("Fixed",pw->variableList[i].vector.at(j).fixed);
                    if(pw->variableList[i].vector.at(j).fixed)
                    {
                        settings.setValue("Length",pw->variableList[i].vector.at(j).length);
                    }
                    settings.setValue("Match",pw->variableList[i].vector.at(j).match);
                    if(pw->variableList[i].vector.at(j).match)
                    {
                        qDebug() << pw->variableList[i].vector.at(j).matchBytes;
                        settings.setValue("MBytes",pw->variableList[i].vector.at(j).matchBytes);
                    }

                }
                settings.endArray();
            }
            else
            {
                // Save an individual complex variable
                settings.setValue("Fixed",pw->variableList[i].fixed);
                if(pw->variableList[i].fixed)
                {
                    settings.setValue("Length",pw->variableList[i].length);
                }
                settings.setValue("Match",pw->variableList[i].match);
                if(pw->variableList[i].match)
                {
                    settings.setValue("MBytes",pw->variableList[i].matchBytes);
                }
            }
        }

        settings.endArray();
        //        settings.endGroup();
        parserCount++;
    }
    settings.endArray();

    // Save Plotters
    //    settings.beginWriteArray("Plotters");
    //    for(int i=0; i<plotters->plotterList.size();i++)
    //    {
    //        settings.setArrayIndex(i);
    //        // Set connection and parser assigned to each plotter
    //        settings.setValue("Connection",plotters->plotterList.at(i)->currentConnection());
    //        settings.setValue("Parser",plotters->plotterList.at(i)->currentParser());
    //    }
    //    settings.endArray();

    settings.setValue("DOCK_LOCATIONS",saveState(1));

    settings.sync();
}

