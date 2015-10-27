#include "plotterwidget.h"

PlotterWidget::PlotterWidget(QList<LinkerUnit*> *linkerList) :
    linkers(linkerList)
{
    //    linkers = linkerList;
    linkerUnit = new LinkerUnit;
    plotTimer = new QTimer;
        plotTimer->setInterval(40);
    plotTimer->start();
    setupUI();

    xval = 0;
    //        yAxisAutoRange = false;
//    connect(plotTimer,SIGNAL(timeout()),this,SLOT(updatePlot()));
}

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
//         QCPGraph *graph = customPlot->graph(0);
//         graph->rescaleValueAxis();
//        customPlot->replot();
//    glPlot->update();
}

void PlotterWidget::newData()
{
    // Add data to plot:
    LinkerUnit *linker = qobject_cast<LinkerUnit *>(QObject::sender());

    ParsedVariable pVar = linker->results.at(variableBox->currentIndex());

    if(pVar.type == NUMTYPE)
    {

        // CustomPlot:
//        QCPGraph *graph = customPlot->graph(0);
//        double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
//        graph->addData(key, linker->results.at(variableBox->currentIndex()).value);
//        graph->removeDataBefore(key-5);
//        customPlot->xAxis->setRange(key+0.1, 5, Qt::AlignRight);
//        graph->rescaleValueAxis();
//        customPlot->replot();

        // GLPlotterWidget:
        glPlot->addDataPoint(linker->results.at(variableBox->currentIndex()).value);
    }
}

void PlotterWidget::setupUI()
{
    linkerBox = new QComboBox;
    variableBox = new QComboBox;

    glPlot = new GLPlotterWidget;

    //  customPlot-only:
//    customPlot = new QCustomPlot;
//    customPlot->addGraph();
//    QPen mypen;
//    mypen.setWidth(1);
//    mypen.setColor(QColor(100,100,250));
//    customPlot->graph(0)->setPen(mypen);
//    customPlot->setBackground(QBrush("black"));
//    QPen gridPen = customPlot->xAxis->grid()->pen();
//    gridPen.setColor(QColor(50,50,50));
//    customPlot->xAxis->grid()->setPen(gridPen);
//    customPlot->yAxis->grid()->setPen(gridPen);
//    gridPen = customPlot->xAxis->basePen();
//    gridPen.setColor(QColor(50,50,50));
//    customPlot->xAxis->setBasePen(gridPen);
//    customPlot->yAxis->setBasePen(gridPen);
//    customPlot->xAxis->setTickPen(gridPen);
//    customPlot->yAxis->setTickPen(gridPen);
//    customPlot->xAxis->setTickLabelColor(QColor(200,200,200));
//    customPlot->yAxis->setTickLabelColor(QColor(200,200,200));
//    customPlot->xAxis->setSubTickPen(gridPen);
//    customPlot->yAxis->setSubTickPen(gridPen);
//    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
//    customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
//    customPlot->xAxis->setAutoTickStep(false);
//    customPlot->xAxis->setTickStep(5);

    dataSourceLayout = new QHBoxLayout;

    dataSourceLayout->addWidget(linkerBox);
    dataSourceLayout->addWidget(variableBox);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(dataSourceLayout);

//    mainLayout->addWidget(customPlot);
    mainLayout->addWidget(glPlot);
    this->setLayout(mainLayout);

    connect(linkerBox,SIGNAL(activated(int)),this,SLOT(updateVariableBox()));
}
