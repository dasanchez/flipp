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

    // Shortcuts
    //    QShortcut *shortcut = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save")));


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

    settings.beginWriteArray("Terminals");

    settings.setArrayIndex(0);
    settings.setValue("Connection","GPS & Time");
    settings.setValue("Echo",true);

    //    settings.beginGroup("01");
    //    settings.setValue("Connection","GPS & time");
    //    settings.setValue("Echo",true);
    //    settings.endGroup();

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
    settings.beginGroup("Parsers");
    parserNames = settings.childGroups();
    foreach(QString parserName,parserNames)
    {
        ParserWidget *pw = new ParserWidget;
        pw->setName(parserName);
        parserNames.append(pw->getName());
        settings.beginGroup(parserName);
        int varSize = settings.beginReadArray("Complex");
        for(int i=0;i<varSize;i++)
        {
            settings.setArrayIndex(i);
            VariableWidget *vw = new VariableWidget;
//            vw->itemList->clear();
            vw->setName(settings.value("Name").toString());
            vw->setType(settings.value("Type").toInt());
            if(settings.value("Type").toInt()==VECTYPE)
            {
                vw->setRepeat(settings.value("Repeat").toInt());
                int vecSize = settings.beginReadArray("Base");
                for(int j=0;j<vecSize;j++)
                {
                    settings.setArrayIndex(j);

                    // Get variable features and initialize each vector item with them:
                    BaseVariable *bv = new BaseVariable;
                    bv->name = settings.value("Name").toString();
                    if(settings.value("Fixed").toBool())
                    {
                        bv->fixed = true;
                        bv->length = settings.value("Length").toInt();
                    }
                    else
                    {
                        bv->fixed = false;
                    }

                    if(settings.value("Type").toInt()==NUMTYPE)
                    {
                        bv->type = NUMTYPE;

                    }
                    else
                    {
                        bv->type = BYTTYPE;
                        if(settings.value("Match").toBool())
                        {
                            bv->match = true;
                            bv->matchBytes = settings.value("MBytes").toByteArray();
                        }
                    }
                    vw->addVectorVariable(bv);
//                    vw->variable->vector->append(bv);
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

        settings.endGroup();
        parsers->addParser(pw);
    }
    settings.endGroup();


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
    settings.beginWriteArray("Terminals");

    for(int i=0;i<terminals->terminalList.size();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("Connection",terminals->terminalList.at(i)->currentConnection());
    }
    settings.endArray();

    // Parsers
    settings.beginGroup("Parsers");
    foreach(ParserWidget *pw, *parsers->parserList)
    {
        settings.beginGroup(pw->getName());

        // Save an array of variable widgets

        settings.beginWriteArray("Complex");
        for(int i=0;i<pw->variableList->size();i++)
        {
            settings.setArrayIndex(i);
            settings.setValue("Name",pw->variableList->at(i)->name);
            settings.setValue("Type",pw->variableList->at(i)->type);
            if(pw->variableList->at(i)->type==VECTYPE)
            {
                settings.setValue("Repeat",pw->variableList->at(i)->repeat);
                // Save an array of base variables
                settings.beginWriteArray("Base");
                for(int j=0;j<pw->variableList->at(i)->vector->size();j++)
                {
                    settings.setArrayIndex(j);
//                    if(settings.va)

                    settings.setValue("Name",pw->variableList->at(i)->vector->at(j)->name);
                    settings.setValue("Type",pw->variableList->at(i)->vector->at(j)->type);
                    settings.setValue("Fixed",pw->variableList->at(i)->vector->at(j)->fixed);
                    if(pw->variableList->at(i)->vector->at(j)->fixed)
                    {
                        settings.setValue("Length",pw->variableList->at(i)->vector->at(j)->length);
                    }
                    settings.setValue("Match",pw->variableList->at(i)->vector->at(j)->match);
                    if(pw->variableList->at(i)->vector->at(j)->match)
                    {
                        qDebug() << pw->variableList->at(i)->vector->at(j)->matchBytes;
                        settings.setValue("MBytes",pw->variableList->at(i)->vector->at(j)->matchBytes);
                    }

                }
                settings.endArray();
            }
            else
            {
                // Save an individual complex variable
                settings.setValue("Fixed",pw->variableList->at(i)->fixed);
                if(pw->variableList->at(i)->fixed)
                {
                    settings.setValue("Length",pw->variableList->at(i)->length);
                }
                settings.setValue("Match",pw->variableList->at(i)->match);
                if(pw->variableList->at(i)->match)
                {
                    settings.setValue("MBytes",pw->variableList->at(i)->matchBytes);
                }
            }
        }

        settings.endArray();
        settings.endGroup();
    }
    settings.endGroup();

    settings.sync();
}
