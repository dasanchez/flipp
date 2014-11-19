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

    plotterList.append(plotWidget);
    splitter->addWidget(plotWidget);

    connect(plotWidget,SIGNAL(removePlotter()),this,SLOT(plotterRemoved()));
}

void PlotterListWidget::plotterRemoved()
{
    PlotterWidget* plotter = qobject_cast<PlotterWidget *>(QObject::sender());

    plotterList.removeAt(plotterList.indexOf(plotter));
    plotter->deleteLater();
}


