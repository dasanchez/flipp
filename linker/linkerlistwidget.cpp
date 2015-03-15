#include "linkerlistwidget.h"

LinkerListWidget::LinkerListWidget(QWidget *parent, QList<LinkerUnit*> *lUnits) :
    QWidget(parent),
    linkers(lUnits)
{
    linkerList = new QList<LinkerWidget*>;
    setupUI();
}

void LinkerListWidget::newLinker()
{
    LinkerWidget *linker = new LinkerWidget;

    linker->updateConnections(connectionNamesList);
    linker->updateParsers(parserNamesList);

    connect(linker,SIGNAL(linkerConnectionRequest(QString)),this,SLOT(linkerRequestedConnection(QString)));
    connect(linker,SIGNAL(linkerParserRequest(QString)),this,SLOT(linkerRequestedParser(QString)));
    connect(linker,SIGNAL(removeLinker()),this,SLOT(linkerRemoved()));

//    qDebug() << "Linker lists: " << linkerList.count();

    if(connectionNamesList.count()>0)
    {
        linker->changeConnection(connectionNamesList.at(0));
    }

    if(parserNamesList.count()>0)
    {
    linker->changeParser(parserNamesList.at(0));
    }
    linkerList->append(linker);
    splitter->addWidget(linker);

    emit linkerListChanged(linkerList);

//    connect(parser,SIGNAL(nameChange()),this,SLOT(nameChanged()));
//    connect(parser,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
//    connect(parser,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
//    updateList();
}

void LinkerListWidget::addLinker(LinkerWidget *liw)
{
    linkerList->append(liw);
    splitter->addWidget(liw);

    connect(liw,SIGNAL(linkerConnectionRequest(QString)),this,SLOT(linkerRequestedConnection(QString)));
    connect(liw,SIGNAL(linkerParserRequest(QString)),this,SLOT(linkerRequestedParser(QString)));
    connect(liw,SIGNAL(removeLinker()),this,SLOT(linkerRemoved()));

//    qDebug() << "Linker lists: " << linkerList.count();

    emit linkerListChanged(linkerList);
}

void LinkerListWidget::linkerRemoved()
{
    LinkerWidget* linker = qobject_cast<LinkerWidget *>(QObject::sender());
    disconnect(linker,SIGNAL(linkerConnectionRequest(QString)),this,SLOT(linkerRequestedConnection(QString)));
    disconnect(linker,SIGNAL(linkerParserRequest(QString)),this,SLOT(linkerRequestedParser(QString)));
    linkerList->removeAt(linkerList->indexOf(linker));
    linker->deleteLater();
//    qDebug() << "Linker lists: " << linkerList.count();
    emit linkerListChanged(linkerList);
}

void LinkerListWidget::updateConnections(QStringList connectionNames)
{
    connectionNamesList = connectionNames;
    foreach(LinkerWidget *linker, *linkerList)
    {
        linker->updateConnections(connectionNamesList);
    }
}

void LinkerListWidget::updateParsers(QStringList parserNames)
{
    parserNamesList = parserNames;
    foreach(LinkerWidget *linker, *linkerList)
    {
        linker->updateParsers(parserNamesList);
    }
}

void LinkerListWidget::linkerRequestedConnection(QString connectionName)
{
    LinkerWidget *linker = qobject_cast<LinkerWidget *>(QObject::sender());
    emit linkerConnectionRequest(linker->linkerUnit,connectionName);
}

void LinkerListWidget::linkerRequestedParser(QString parserName)
{
    LinkerWidget *linker = qobject_cast<LinkerWidget *>(QObject::sender());
    emit linkerParserRequest(linker->linkerUnit,parserName);
}

void LinkerListWidget::setupUI()
{
    widgetNameLabel = new QLabel("Linkers");

    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);

    newLinkerButton = new QPushButton("New");
    newLinkerButton->setFont(font);
//    newLinkerButton->setFixedHeight(24);
    newLinkerButton->setFixedWidth(90);

    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    splitter->setHandleWidth(1);

    scrollArea = new QScrollArea;
    scrollAreaVLayout = new QVBoxLayout;
    scrollAreaVLayout->setSpacing(0);
    scrollAreaVLayout->setMargin(0);
    saWidgetContents = new QWidget();

    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(saWidgetContents);
    scrollArea->setWidgetResizable(true);
    scrollAreaVLayout->addWidget(splitter);
    saWidgetContents->setLayout(scrollAreaVLayout);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newLinkerButton);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);

    setMinimumWidth(500);
    connect(newLinkerButton,SIGNAL(clicked()),this,SLOT(newLinker()));
}
