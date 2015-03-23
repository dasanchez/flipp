#include "plotterwidget.h"

//PlotterWidget::PlotterWidget(QWidget *parent, QList<LinkerUnit*> *linkerList) :
//    QWidget(parent),
//    linkers(linkerList)

PlotterWidget::PlotterWidget(QList<LinkerUnit*> *linkerList) :
    linkers(linkerList)
{
    //    linkers = linkerList;
    linkerUnit = new LinkerUnit;
    plotTimer = new QTimer;
    plotTimer->setInterval(20);
    plotTimer->start();
    setupUI();

    xval = 0;
    //        yAxisAutoRange = false;
    connect(plotTimer,SIGNAL(timeout()),this,SLOT(updatePlot()));
//    connect(plotTimer,SIGNAL(timeout()),this,SLOT(newSimData()));
}

//PlotterWidget::PlotterWidget(QWidget *parent)
//    : QWidget(parent)
//{
//    plotTimer = new QTimer;
//    plotTimer->setInterval(10);
//    plotTimer->start();
//    setupUI();
//    //    yAxisAutoRange = false;
//    connect(plotTimer,SIGNAL(timeout()),this,SLOT(updatePlot()));
//}

PlotterWidget::~PlotterWidget()
{

}

void PlotterWidget::updateLinkerList()
{

    //    linkerList = list;
    //    qDebug() << "Linker lists: " << list.count();
    linkerBox->clear();
    foreach(LinkerUnit *lUnit, *linkers)
    {
        linkerBox->addItem(lUnit->getName());
    }
    updateVariableBox();

}

void PlotterWidget::updateVariableBox()
{
    disconnect(linkerUnit,SIGNAL(newDataPoint()),this,SLOT(newData()));
    if(linkers->count()>0)
    {
        linkerUnit = linkers->at(linkerBox->currentIndex());

        variableBox->clear();
        for(quint8 i=0;i<linkerUnit->getVariables().size();i++)
        {
            variableBox->addItem(linkerUnit->getVariables().at(i).name);
        }
    }
    connect(linkerUnit,SIGNAL(newDataPoint()),this,SLOT(newData()));
}

void PlotterWidget::updatePlot()
{
    //     QCPGraph *graph = customPlot->graph(0);
    //     graph->rescaleValueAxis();
    customPlot->replot();
}

void PlotterWidget::newSimData()
{
    QCPGraph *graph = customPlot->graph(0);
    xval+=.01;
    if(xval>=6.28)
    {
        xval=0;
    }
    yval=sin(xval);
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    graph->addData(key, yval);
    graph->removeDataBefore(key-20);
    customPlot->xAxis->setRange(key+0.1, 20, Qt::AlignRight);
    graph->rescaleValueAxis();
    customPlot->replot();

}

void PlotterWidget::newData()
{
    QCPGraph *graph = customPlot->graph(0);
    //qDebug() << "updating plot, data point:" << dataPoint.name << ", current variable:" << variableBox->currentText();
    // add data to lines:
    LinkerUnit *linker = qobject_cast<LinkerUnit *>(QObject::sender());

    //    qDebug() << "type: " << linker->results.at(variableBox->currentIndex()).type;

    //    qDebug() << "Linker results: " << linker->results.size() << ", variable box: " << variableBox->count();
    ParsedVariable pVar = linker->results.at(variableBox->currentIndex());
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    //    if(linker->results.at(variableBox->currentIndex()).type == NUMTYPE)
    if(pVar.type == NUMTYPE)
    {
        graph->addData(key, linker->results.at(variableBox->currentIndex()).value);
        graph->removeDataBefore(key-20);
        customPlot->xAxis->setRange(key+0.1, 20, Qt::AlignRight);
        //        customPlot->replot();
        graph->rescaleValueAxis();
    }
}

//void PlotterWidget::populatePlotArea()
//{
//    // Setup value vector
//    valuesList.clear();

//    // Add a layer for each number value
//    customPlot->clearGraphs();

//            // Add a graph for each number value
//            customPlot->addGraph();
//            QPen pen;
//            pen.setWidth(1);
//            pen.setColor(QColor(255,0,0));
//            customPlot->graph(customPlot->graphCount()-1)->setPen(pen);

//            customPlot->setNoAntialiasingOnDrag(true);
//}

//void PlotterWidget::toggleSettingsVisible(bool on)
//{
//    xAxisSettingsBox->setVisible(on);
//    yAxisSettingsBox->setVisible(on);
//    if(on)
//    {
//        showPlotSettingsButton->setText("Hide settings");
//    }
//    else
//    {
//        showPlotSettingsButton->setText("Show settings");
//    }
//}

//void PlotterWidget::adjustXRange()
//{
//    // Adjust the valuesList QList to reflect the new xMax value.
//    // Two scenarios:
//    // 1. The new xMax is larger than the existing valuesList QList
//    // 2. The new xMax is smaller than the existing valueList QList
//    if(valuesList.size()>0)
//    {
//        if((quint16)xMax > valuesList.at(0).size())
//        {
//            quint16 newZeroes = (quint16) xMax -valuesList.at(0).size();
//            for(int i=0;i<valuesList.size();i++)
//            {
//                for(quint16 j=0;j<newZeroes;j++)
//                {
//                    valuesList[i].prepend(0);
//                }
//            }
//        }
//        else
//        {
//            quint16 lessPoints = valuesList.at(0).size()- (quint16) xMax;
//            for(int i=0;i<valuesList.size();i++)
//            {
//                valuesList[i].remove(0,lessPoints);
//            }
//        }
//    }
//}

//void PlotterWidget::setXRange(double newMax)
//{
//    xMax=newMax;

//    keys.clear();
//    for(int i=0;i<(quint16) xMax;i++)
//    {
//        keys.append(i);
//    }
//}

//void PlotterWidget::togglexAxisAutoTicks(bool autotick)
//{
//    customPlot->xAxis->setAutoTickStep(autotick);
//    if(!autotick)
//    {
//    customPlot->xAxis->setTickStep(xTicksSpin->value());
//    }
//    else
//    {
//        customPlot->axisRect()->setupFullAxesBox();
//    }
//}

//void PlotterWidget::xTicksChanged(int ticks)
//{
//    if(!customPlot->xAxis->autoTickStep())
//    {
//        customPlot->xAxis->setTickStep(ticks);
//    }
//}

//void PlotterWidget::toggleyAxisAutoRange(bool on)
//{
//    yAxisAutoRange = on;
//    if(on)
//    {
//        for(int i=0;i<customPlot->graphCount();i++)
//        {
//            customPlot->graph(i)->rescaleValueAxis();
//        }
//    }
//    else
//    {
//        customPlot->yAxis->setRange(yMin,yMax);
//    }
//}

//void PlotterWidget::setYMin(QString newMin)
//{
//    yMin = newMin.toDouble();
//    disconnect(yMinSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMin(QString)));
//    yMinSpin->setValue(yMin);
//    connect(yMinSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMin(QString)));
//    customPlot->yAxis->setRangeLower(yMin);
//    customPlot->replot();
//}

//void PlotterWidget::setYMax(QString newMax)
//{
//    yMax = newMax.toDouble();
//    disconnect(yMaxSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMax(QString)));
//    yMaxSpin->setValue(yMax);
//    connect(yMaxSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMax(QString)));
//    customPlot->yAxis->setRangeUpper(yMax);
//    customPlot->replot();
//}

void PlotterWidget::setupUI()
{
    //    delIconPixmap = QPixmap(":/images/delete_icon.png");
    //    xMax=100;
    //    yMin = 0;
    //    yMax = 70;
    //    for(quint16 i=0;i<(quint16) xMax;i++)
    //    {
    //        keys.append(i);
    //    }
    linkerBox = new QComboBox;
    variableBox = new QComboBox;

    customPlot = new QCustomPlot;
    //    customPlot->setUpdatesEnabled(true);
    //    customPlot->yAxis->setRange(yMin,yMax);

    customPlot->addGraph();
    QPen mypen;
    mypen.setWidth(1);
    mypen.setColor(QColor(100,100,250));
    customPlot->graph(0)->setPen(mypen);


    customPlot->setBackground(QBrush("black"));

    QPen gridPen = customPlot->xAxis->grid()->pen();
    gridPen.setColor(QColor(50,50,50));
    customPlot->xAxis->grid()->setPen(gridPen);
    customPlot->yAxis->grid()->setPen(gridPen);

    gridPen = customPlot->xAxis->basePen();
    gridPen.setColor(QColor(50,50,50));

    customPlot->xAxis->setBasePen(gridPen);
    customPlot->yAxis->setBasePen(gridPen);
    //customPlot->xAxis->setLabelColor(QColor(50,50,50));
    //customPlot->yAxis->setLabelColor(QColor(50,50,50));
    customPlot->xAxis->setTickPen(gridPen);
    customPlot->yAxis->setTickPen(gridPen);
    customPlot->xAxis->setTickLabelColor(QColor(200,200,200));
    customPlot->yAxis->setTickLabelColor(QColor(200,200,200));
    customPlot->xAxis->setSubTickPen(gridPen);
    customPlot->yAxis->setSubTickPen(gridPen);

    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);

    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("hh:mm:ss");

    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(5);
    //customPlot->axisRect()->setupFullAxesBox();

    //    showPlotSettingsButton = new QPushButton("Hide Settings");
    //    showPlotSettingsButton->setFixedHeight(24);
    //    showPlotSettingsButton->setCheckable(true);
    //    showPlotSettingsButton->setChecked(true);
    //    showPlotSettingsButton->setObjectName("showPlotSettingsBtn");

    //    xRangeLabel = new QLabel("History");
    //    xRangeLabel->setFixedHeight(24);
    //    xRangeSpin = new QDoubleSpinBox;
    //    xRangeSpin->setFixedHeight(24);
    //    xRangeSpin->setDecimals(0);
    //    xRangeSpin->setValue(xMax);
    //    xRangeSpin->setMinimum(1);
    //    xRangeSpin->setMaximum(10000);

    //    xTicksLabel = new QLabel("Ticks");
    //    xTicksLabel->setFixedHeight(24);
    //    xTicksButton = new QPushButton("Auto");
    //    xTicksButton->setFixedHeight(24);
    //    xTicksButton->setObjectName("xTicksButton");
    //    xTicksButton->setCheckable(true);
    //    xTicksButton->setChecked(false);
    //    xTicksSpin = new QSpinBox;
    //    xTicksSpin->setFixedHeight(24);
    //    xTicksSpin->setValue(5);
    //    xTicksSpin->setMinimum(1);
    //    xTicksSpin->setMaximum(20);

    //    yAxisRangeButton = new QPushButton("Auto-range");
    //    yAxisRangeButton->setFixedHeight(24);
    //    yAxisRangeButton->setCheckable(true);
    //    yAxisRangeButton->setChecked(false);
    //    yAxisRangeButton->setObjectName("yAxisRangeButton");

    //    yMinLabel = new QLabel("Y Min");
    //    yMinLabel->setFixedHeight(24);
    //    yMinSpin = new QSpinBox;
    //    yMinSpin->setFixedHeight(24);
    //    yMinSpin->setValue(yMin);
    //    yMinSpin->setMinimum(-100000);
    //    yMinSpin->setMaximum(100000);

    //    yMaxLabel = new QLabel("Y Max");
    //    yMaxLabel->setFixedHeight(24);
    //    yMaxSpin = new QSpinBox;
    //    yMaxSpin->setFixedHeight(24);
    //    yMaxSpin->setValue(yMax);
    //    yMaxSpin->setMinimum(-100000);
    //    yMaxSpin->setMaximum(100000);

    //    tableWidget = new QTableWidget(this);
    //    tableWidget->setColumnCount(3);
    //    QStringList tableHeaders;
    //    tableHeaders.append("Field");
    //    tableHeaders.append("Plot");
    //    tableHeaders.append("Value");
    //    tableWidget->setHorizontalHeaderLabels(tableHeaders);
    //    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    //    QHeaderView *hv = tableWidget->horizontalHeader();
    //    hv->setStretchLastSection(true);
    //    hv->setSectionsClickable(false);
    //    hv->setFixedHeight(24);
    //    hv->setSectionResizeMode(1,QHeaderView::ResizeToContents);

    //    hv = tableWidget->verticalHeader();
    //    hv->setSectionsClickable(false);

    dataSourceLayout = new QHBoxLayout;

    //    xRangeLayout = new QHBoxLayout;
    //    xRangeLayout->addWidget(xRangeLabel);
    //    xRangeLayout->addWidget(xRangeSpin);

    //    xTicksLayout = new QHBoxLayout;
    //    xTicksLayout->addWidget(xTicksLabel);
    //    xTicksLayout->addWidget(xTicksButton);
    //    xTicksLayout->addWidget(xTicksSpin);

    //    xAxisLayout = new QVBoxLayout;
    //    xAxisLayout->addLayout(xRangeLayout);
    //    xAxisLayout->addLayout(xTicksLayout);

    //    xAxisSettingsBox = new QGroupBox("X Axis");
    //    xAxisSettingsBox->setLayout(xAxisLayout);
    //    xAxisSettingsBox->setFixedHeight(100);

    //    yAxisMinLayout = new QHBoxLayout;
    //    yAxisMinLayout->addWidget(yMinLabel);
    //    yAxisMinLayout->addWidget(yMinSpin);
    //    yAxisMaxLayout = new QHBoxLayout;
    //    yAxisMaxLayout->addWidget(yMaxLabel);
    //    yAxisMaxLayout->addWidget(yMaxSpin);

    //    yAxisLayout = new QVBoxLayout;
    //    yAxisLayout->addWidget(yAxisRangeButton);
    //    yAxisLayout->addLayout(yAxisMinLayout);
    //    yAxisLayout->addLayout(yAxisMaxLayout);
    //    yAxisSettingsBox = new QGroupBox("Y Axis");
    //    yAxisSettingsBox->setLayout(yAxisLayout);
    //    yAxisSettingsBox->setFixedHeight(130);

    //    plotSettingsLayout = new QVBoxLayout;
    //    plotSettingsLayout->addWidget(xAxisSettingsBox);
    //    plotSettingsLayout->addWidget(yAxisSettingsBox);

    //    plotLayout = new QVBoxLayout;
    //    plotLayout->addWidget(customPlot);
    //    plotLayout->addWidget(showPlotSettingsButton);
    //    plotLayout->addLayout(plotSettingsLayout);

    //    QWidget *plotWidgetContainer = new QWidget;
    //    plotWidgetContainer->setLayout(plotLayout);

    //    tableLayout = new QHBoxLayout;
    //    tableLayout->addWidget(tableWidget);

    //    QWidget *tableWidgetContainer = new QWidget;
    //    tableWidgetContainer->setLayout(tableLayout);

    //    contentSplitter = new QSplitter(this);
    //    contentSplitter->addWidget(tableWidgetContainer);
    //    contentSplitter->addWidget(plotWidgetContainer);

    dataSourceLayout->addWidget(linkerBox);
    dataSourceLayout->addWidget(variableBox);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(dataSourceLayout);
    //    mainLayout->addLayout(dataSourceLayout);
    //    mainLayout->addWidget(contentSplitter);

    mainLayout->addWidget(customPlot);
    this->setLayout(mainLayout);

    connect(linkerBox,SIGNAL(activated(int)),this,SLOT(updateVariableBox()));

    //    connect(removeButton,SIGNAL(clicked()),this,SIGNAL(removePlotter()));
    //    connect(xRangeSpin,SIGNAL(valueChanged(double)),this,SLOT(setXRange(double)));
    //    connect(xTicksButton,SIGNAL(clicked(bool)),this,SLOT(togglexAxisAutoTicks(bool)));
    //    connect(xTicksSpin,SIGNAL(valueChanged(int)),this,SLOT(xTicksChanged(int)));
    //    connect(yMinSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMin(QString)));
    //    connect(yMaxSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMax(QString)));
    //    connect(yAxisRangeButton,SIGNAL(clicked(bool)),this,SLOT(toggleyAxisAutoRange(bool)));
    //    connect(showPlotSettingsButton,SIGNAL(clicked(bool)),this,SLOT(toggleSettingsVisible(bool)));
}
