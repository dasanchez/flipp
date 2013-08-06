#include "plotterlistwidget.h"

PlotterListWidget::PlotterListWidget(QWidget *parent) :
    QWidget(parent)
{
    widgetNameLabel = new QLabel("Plots");
    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);
    newPlotterButton = new QPushButton("New");
    newPlotterButton->setFixedHeight(24);
    newPlotterButton->setFixedWidth(100);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newPlotterButton);

    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);

    scrollArea = new QScrollArea;
    scrollAreaVLayout = new QVBoxLayout;
    saWidgetContents = new QWidget();

    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(saWidgetContents);
    scrollArea->setWidgetResizable(true);
    scrollAreaVLayout->addWidget(splitter);
    saWidgetContents->setLayout(scrollAreaVLayout);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    this->setLayout(mainLayout);

    setMinimumWidth(480);
    connect(newPlotterButton,SIGNAL(clicked()),this,SLOT(newPlotter()));
}

PlotterListWidget::~PlotterListWidget()
{

}

void PlotterListWidget::newPlotter()
{
    PlotterWidget *plotWidget = new PlotterWidget;

    plotWidget->updateConnections(connectionNamesList);
    plotWidget->updateParsers(parserNamesList);
    plotterList.append(plotWidget);
    splitter->addWidget(plotWidget);

    connect(plotWidget,SIGNAL(plotterConnectionRequest(QString)),this,SLOT(plotterRequestedConnection(QString)));
    connect(plotWidget,SIGNAL(plotterParserRequest(QString)),this,SLOT(plotterRequestedParser(QString)));
    connect(plotWidget,SIGNAL(removePlotter()),this,SLOT(plotterRemoved()));
}

void PlotterListWidget::plotterRemoved()
{
    PlotterWidget* plotter = qobject_cast<PlotterWidget *>(QObject::sender());
    disconnect(plotter,SIGNAL(plotterConnectionRequest(QString)),this,SLOT(plotterRequestedConnection(QString)));
    disconnect(plotter,SIGNAL(plotterParserRequest(QString)),this,SLOT(plotterRequestedParser(QString)));
    plotterList.removeAt(plotterList.indexOf(plotter));
    plotter->deleteLater();
}

void PlotterListWidget::updateConnections(QStringList connectionNames)
{
    connectionNamesList = connectionNames;
    foreach(PlotterWidget *plotter,plotterList)
    {
        plotter->updateConnections(connectionNamesList);
    }
}

void PlotterListWidget::updateParsers(QStringList parserNames)
{
    parserNamesList = parserNames;
    foreach(PlotterWidget *plotter,plotterList)
    {
        plotter->updateParsers(parserNamesList);
    }
}

void PlotterListWidget::plotterRequestedConnection(QString connectionName)
{
    PlotterWidget* plotter = qobject_cast<PlotterWidget *>(QObject::sender());
    emit plotterConnectionRequest(plotter,connectionName);
}

void PlotterListWidget::plotterRequestedParser(QString parserName)
{
    PlotterWidget* plotter = qobject_cast<PlotterWidget *>(QObject::sender());
    emit plotterParserRequest(plotter,parserName);
}
