#include "flipp.h"

Flipp::Flipp(QWidget *parent)
    : QMainWindow(parent)
{
    connections = new QList<ConnectionUnit*>;
    connectionListWidget = new ConnectionListWidget(this, connections);
    terminals = new TerminalListWidget(this);
    parsers = new QList<ParserUnit*>;
    parserListWidget = new ParserListWidget(this, parsers);
    linkers = new QList<LinkerUnit*>;
    linkerListWidget = new LinkerListWidget(this, linkers);
    plotter = new PlotterWidget(this,linkers);
    //    plotter = new PlotterWidget(this);

    m_sSettingsFile = QApplication::applicationDirPath() + "/lastSettings.flp";
    setCentralWidget(plotter);
    createDocks();
    createMenus();

    this->setWindowTitle(tr("f l i p p"));

    QFile qss("../flipp/styles/flipp.css");
    qss.open(QFile::ReadOnly);
    setStyleSheet(qss.readAll());
    qss.close();

    connect(connectionListWidget,SIGNAL(connectionListChanged(QStringList)),terminals,SLOT(updateConnections(QStringList)));
    connect(connectionListWidget,SIGNAL(connectionListChanged(QStringList)),linkerListWidget,SLOT(updateConnections(QStringList)));
    connect(terminals,SIGNAL(terminalRequest(TerminalWidget*,QString)),this,SLOT(handleTerminalRequest(TerminalWidget*,QString)));
    connect(parserListWidget,SIGNAL(parserListChanged(QStringList)),linkerListWidget,SLOT(updateParsers(QStringList)));

    connect(linkerListWidget,SIGNAL(linkerConnectionRequest(LinkerUnit*,QString)),this,SLOT(handleLinkerConnectionRequest(LinkerUnit*, QString)));
    connect(linkerListWidget,SIGNAL(linkerParserRequest(LinkerUnit*,QString)),this,SLOT(handleLinkerParserRequest(LinkerUnit*, QString)));

    connect(linkerListWidget,SIGNAL(linkerListChanged()),plotter,SLOT(updateLinkerList()));

    restoreSettings();
}

Flipp::~Flipp()
{

}

void Flipp::handleLinkerConnectionRequest(LinkerUnit* linker, QString name)
{
    for(quint8 i=0;i<connections->count();i++)
    {
        if(connections->at(i)->getName()==name)
        {
            linker->assignConnection(connections->at(i));
        }
    }
}

void Flipp::handleLinkerParserRequest(LinkerUnit* linker, QString name)
{
    foreach(ParserUnit *parser,*parsers)
    {
        if(parser->getName()==name && parser->hasValidName())
        {
            linker->assignParser(parser);
        }
    }
}

void Flipp::handleTerminalRequest(TerminalWidget *terminal,QString name)
{
    foreach(ConnectionWidget *connection,connectionListWidget->connectionList)
    {
        if(connection->getName()==name)
        {
            terminal->assignConnection(connection->connectionUnit);
        }
    }
}

void Flipp::createDocks()
{
    connectionDock = new QDockWidget(tr("Connection list"));
    connectionDock->setObjectName("Connections_Dock");
    connectionDock->setWidget(connectionListWidget);
    connectionDock->setFeatures(QDockWidget::DockWidgetClosable|
                                QDockWidget::DockWidgetMovable|
                                QDockWidget::DockWidgetFloatable);
    connectionDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    addDockWidget(Qt::TopDockWidgetArea,connectionDock);

    terminalDock = new QDockWidget(tr("Terminal list"));
    terminalDock->setObjectName("Terminals_Dock");
    terminalDock->setWidget(terminals);
    terminalDock->setFeatures(QDockWidget::DockWidgetClosable|
                              QDockWidget::DockWidgetMovable|
                              QDockWidget::DockWidgetFloatable);
    terminalDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea,terminalDock);

    parserDock = new QDockWidget(tr("Parser list"));
    parserDock->setObjectName("Parsers_Dock");
    parserDock->setWidget(parserListWidget);
    parserDock->setFeatures(QDockWidget::DockWidgetClosable|
                            QDockWidget::DockWidgetMovable|
                            QDockWidget::DockWidgetFloatable);
    parserDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea,parserDock);

    linkerDock = new QDockWidget(tr("Linker list"));
    linkerDock->setObjectName("linkerListWidget_Dock");
    linkerDock->setWidget(linkerListWidget);
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

    terminalDock->toggleViewAction()->setShortcut(QKeySequence(tr("Alt+W")));
    viewMenu->addAction(terminalDock->toggleViewAction());

    parserDock->toggleViewAction()->setShortcut(QKeySequence(tr("Alt+E")));
    viewMenu->addAction(parserDock->toggleViewAction());
    linkerDock->toggleViewAction()->setShortcut(QKeySequence(tr("Alt+R")));
    viewMenu->addAction(linkerDock->toggleViewAction());
}

void Flipp::dockWidgets()
{
    connectionDock->setFloating(false);
    terminalDock->setFloating(false);
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
    connectionDock->setVisible(true);

    QStringList connectionNames;
    QStringList parserNames;

    // Restore connection widgets
    int connCount = settings.beginReadArray("Connections");
    for(int i=0;i<connCount;i++)
    {
        settings.setArrayIndex(i);
        ConnectionUnit *connectionUnit = new ConnectionUnit;
        connectionUnit->setName(settings.value("Name").toString());
        connectionUnit->setType((connectionType) settings.value("Type").toInt());
        connectionUnit->setAddress_Port(settings.value("Address").toString());
        connectionUnit->setPort_Baud(settings.value("Port").toString().toLong());

        connections->append(connectionUnit);
        connectionNames.append(connectionUnit->getName());
        connectionListWidget->addConnection(connectionUnit);
    }
    settings.endArray();

    // Restore terminal widgets
    int terminalCount = settings.beginReadArray("Terminals");
    for(int i=0;i<terminalCount;i++)
    {
        settings.setArrayIndex(i);
        TerminalWidget *tw = new TerminalWidget;

        // Add terminal widget to terminal list
        terminals->addTerminal(tw);

        // Set connections
        tw->updateConnections(connectionNames);
        tw->changeConnection(settings.value("Connection").toString());

        // Set views
    }

    settings.endArray();

    // Restore parser widgets
    int parserCount = settings.beginReadArray("Parsers");

    for(int i=0;i<parserCount;i++)
    {
        settings.setArrayIndex(i);
        ParserUnit *pUnit = new ParserUnit;
        parsers->append(pUnit);
        pUnit->setName(settings.value("Name").toString());
        parserNames.append(pUnit->getName());

        int varSize = settings.beginReadArray("Complex");
        for(int j=0;j<varSize;j++)
        {
            settings.setArrayIndex(j);
            ComplexVariable *cVar = new ComplexVariable;
            cVar->name = settings.value("Name").toString();
            cVar->type = settings.value("Type").toInt();

            if(settings.value("Type").toInt()==VECTYPE)
            {
//                /* FIX eventually
//                    vw->setRepeat(settings.value("Repeat").toInt());
//                    int vecSize = settings.beginReadArray("Base");
//                    for(int k=0;k<vecSize;k++)
//                    {
//                        settings.setArrayIndex(k);

//                        // Get variable features and initialize each vector item with them:
//                        BaseVariable bv;// = new BaseVariable;
//                        bv.name = settings.value("Name").toString();
//                        if(settings.value("Fixed").toBool())
//                        {
//                            bv.fixed = true;
//                            bv.length = settings.value("Length").toInt();
//                        }
//                        else
//                        {
//                            bv.fixed = false;
//                        }

//                        if(settings.value("Type").toInt()==NUMTYPE)
//                        {
//                            bv.type = NUMTYPE;

//                        }
//                        else
//                        {
//                            bv.type = BYTTYPE;
//                            if(settings.value("Match").toBool())
//                            {
//                                bv.match = true;
//                                bv.matchBytes = settings.value("MBytes").toByteArray();
//                            }
//                        }
//                        vw->addVectorVariable(bv);
//                    }
//                    settings.endArray();
//                */
            }
            else
            {
                cVar->fixed = settings.value("Fixed").toBool();
                if(cVar->fixed)
                {
                    cVar->length = settings.value("Length").toInt();
                }
                cVar->match = settings.value("Match").toBool();
                if(cVar->match)
                {
                    cVar->matchBytes = settings.value("MBytes").toByteArray();
                }
            }
            pUnit->addVariable(cVar);
        }
        settings.endArray();
//        qDebug() << "Vars in parser " << pUnit->getName() << ":";
//        foreach(ComplexVariable *cVar, *pUnit->variableList)
//        {
//            qDebug() << cVar->name;
//        }
        parserListWidget->addParser(pUnit);
    }
    settings.endArray();

    // Restore linker widgets
    int linkerCount = settings.beginReadArray("Linkers");
    for(int i=0;i<linkerCount;i++)
    {
        settings.setArrayIndex(i);
        LinkerUnit *lUnit = new LinkerUnit;
        linkers->append(lUnit);


//         Restore connection
        for(quint8 j=0;j<connections->count();j++)
        {
            if(connections->at(j)->getName()==settings.value("Connection").toString())
            {
                lUnit->assignConnection(connections->at(j));
                break;
            }
        }
//         Restore variables
        for(quint8 j=0;j<parserCount;j++)
        {
            if(parsers->at(j)->getName()==settings.value("Parser").toString())
            {
                lUnit->assignParser(parsers->at(j));
            }
        }
        linkerListWidget->addLinker(lUnit);
    }
    settings.endArray();

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
    for(int i=0;i<connections->size();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("Name",connections->at(i)->getName());
        int vartype = connections->at(i)->getType();
        settings.setValue("Type",vartype);
        settings.setValue("Address",connections->at(i)->getAddress_Port());
        settings.setValue("Port",connections->at(i)->getPort_Baud());

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

    for(quint8 i=0;i<parsers->size();i++)
    {
        ParserUnit *pUnit = parsers->at(i);
        settings.setArrayIndex(parserCount);
        settings.setValue("Name",pUnit->getName());


        // Save an array of variable widgets

        settings.beginWriteArray("Complex");
        for(quint8 j=0;j<pUnit->variableList->size();j++)
        {
            settings.setArrayIndex(j);
            settings.setValue("Name",pUnit->variableList->at(j)->name);
            settings.setValue("Type",pUnit->variableList->at(j)->type);
            if(pUnit->variableList->at(j)->type == VECTYPE)
            {
                //                settings.setValue("Repeat",pw->variableList[i].repeat);
                //                // Save an array of base variables
                //                settings.beginWriteArray("Base");
                //                for(int j=0;j<pw->variableList[i].vector.size();j++)
                //                {
                //                    settings.setArrayIndex(j);

                //                    settings.setValue("Name",pw->variableList[i].vector.at(j).name);
                //                    settings.setValue("Type",pw->variableList[i].vector.at(j).type);
                //                    settings.setValue("Fixed",pw->variableList[i].vector.at(j).fixed);
                //                    if(pw->variableList[i].vector.at(j).fixed)
                //                    {
                //                        settings.setValue("Length",pw->variableList[i].vector.at(j).length);
                //                    }
                //                    settings.setValue("Match",pw->variableList[i].vector.at(j).match);
                //                    if(pw->variableList[i].vector.at(j).match)
                //                    {
                //                        //                        qDebug() << pw->variableList[i].vector.at(j).matchBytes;
                //                        settings.setValue("MBytes",pw->variableList[i].vector.at(j).matchBytes);
                //                    }

                //                }
                //                settings.endArray();
            }
            else
            {
                // Save an individual complex variable
                settings.setValue("Fixed",pUnit->variableList->at(j)->fixed);

                if(pUnit->variableList->at(j)->fixed)
                {
                    settings.setValue("Length",pUnit->variableList->at(j)->length);
                }
                settings.setValue("Match",pUnit->variableList->at(j)->match);

                if(pUnit->variableList->at(j)->match)
                {
                    settings.setValue("MBytes",pUnit->variableList->at(j)->matchBytes);
                }
            }
        }
//        qDebug() << pUnit->variableList->size() << " vars saved in parser @ flipp";
        settings.endArray();
        parserCount++;
    }
    settings.endArray();

    // Linkers
    settings.beginWriteArray("Linkers");
    int linkerCount=0;
    foreach(LinkerUnit *lUnit, *linkers)
    {
        settings.setArrayIndex(linkerCount);
        settings.setValue("Connection", lUnit->getConnectionName());
        settings.setValue("Parser",lUnit->getParserName());
        linkerCount++;
    }
    settings.endArray();

    settings.setValue("DOCK_LOCATIONS",saveState(1));

    settings.sync();
}

