#include "plotterwidget.h"

PlotterWidget::PlotterWidget(QWidget *parent)
    : QWidget(parent)
{
    parserEngine = new ParserEngine;
    plotTimer = new QTimer;
    plotTimer->setInterval(10);
    plotTimer->start();
    setupUI();
    connect(parserEngine,SIGNAL(dataParsed(QList<RepeatedVector>)),this,SLOT(parsedDataReady(QList<RepeatedVector>)));
    connect(plotTimer,SIGNAL(timeout()),this,SLOT(updatePlot()));

    //    setFixedWidth(410);

}

void PlotterWidget::updatePlot()
{
    customPlot->replot();
}

PlotterWidget::~PlotterWidget()
{

    //    keys.clear();
    //    delete keys;
    //customPlot->deleteLater();
}

QString PlotterWidget::currentConnection()
{
    return connectionWidget->getName();
}

QString PlotterWidget::currentParser()
{
    return parserWidget->getName();
}

void PlotterWidget::updateConnections(QStringList connectionNames)
{
    connectionBox->clear();
    connectionBox->addItems(connectionNames);
    //    int index = connectionBox->findText(connectionWidget->getName());
    //    if(index>=0)
    //        connectionBox->setCurrentIndex(index);
    changeConnection(connectionBox->currentText());
}

void PlotterWidget::updateParsers(QStringList parserNames)
{
    parserBox->clear();
    parserBox->addItems(parserNames);
    //    int index = parserBox->findText(connectionWidget->getName());
    //    if(index>=0)
    //        parserBox->setCurrentIndex(index);
}

void PlotterWidget::changeConnection(QString connection)
{
    emit plotterConnectionRequest(connection);
}

void PlotterWidget::assignConnection(ConnectionWidget *connWidget)
{
    connectionWidget=connWidget;
    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(connectionWidget,SIGNAL(widgetRemoved()),this,SLOT(detachConnection()));
}

void PlotterWidget::detachConnection()
{
    connectionWidget = new ConnectionWidget;
}

void PlotterWidget::changeParser(QString parserName)
{

    emit plotterParserRequest(parserName);
}

void PlotterWidget::assignParser(ParserWidget *parser)
{
    parserWidget = parser;


    connect(parserWidget,SIGNAL(updateVariableList()),this,SLOT(populateParserTable()));
    connect(parserWidget,SIGNAL(updateVariableList()),this,SLOT(populatePlotArea()));
    connect(parserWidget,SIGNAL(deleteParser()),this,SLOT(detachParser()));

    // Populate QTableWidget and QCustomPlot
    populateParserTable();
    populatePlotArea();
}

void PlotterWidget::detachParser()
{
    parserWidget = new ParserWidget;
}

void PlotterWidget::populateParserTable()
{
    //    qDebug() << "populate table";
    //    tableWidget->clear();
    quint8 i=0;
    tableWidget->setRowCount(calcRowCount());
    foreach(ComplexVariable *var,*parserWidget->variableList)
    {
        //        qDebug() << "row: " << i << ", varName: " << var->name;
        if(var->type==VECTYPE)
        {
            QString fullName;
            foreach(BaseVariable *bvar,*var->vector)
            {
                fullName.clear();
                fullName.append(var->name);
                fullName.append(".");
                fullName.append(bvar->name);
                QTableWidgetItem *item = new QTableWidgetItem(fullName);
                item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
                item->setFlags(Qt::NoItemFlags);
                tableWidget->setItem(i,0,item);
                QTableWidgetItem *item2 = new QTableWidgetItem("-");
                item2->setTextAlignment(Qt::AlignBottom);
                item2->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
                item2->setFlags(Qt::NoItemFlags);
                tableWidget->setItem(i,2,item2);
                // Checkbox
                QWidget *container = new QWidget;
                QHBoxLayout *layout = new QHBoxLayout;
                layout->setSpacing(0);
                layout->setMargin(0);
                QCheckBox *checkBox = new QCheckBox;
                layout->addStretch(1);
                layout->addWidget(checkBox);
                layout->addStretch(1);
                container->setLayout(layout);
                tableWidget->setCellWidget(i,1,container);
                i++;

            }
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem(var->name);
            item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            item->setFlags(Qt::NoItemFlags);
            tableWidget->setItem(i,0,item);
            QTableWidgetItem *item2 = new QTableWidgetItem("-");
            item2->setTextAlignment(Qt::AlignBottom);
            item2->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            item2->setFlags(Qt::NoItemFlags);
            tableWidget->setItem(i,2,item2);
            // Checkbox
            QTableWidgetItem *item3 = new QTableWidgetItem;
            item3->setCheckState(Qt::Unchecked);
            tableWidget->setItem(i,1,item3);
            //            QWidget *container = new QWidget;
            //            QHBoxLayout *layout = new QHBoxLayout;
            //            layout->setSpacing(0);
            //            layout->setMargin(0);
            //            QCheckBox *checkBox = new QCheckBox;
            //            layout->addStretch(1);
            //            layout->addWidget(checkBox);
            //            layout->addStretch(1);
            //            container->setLayout(layout);
            //            tableWidget->setCellWidget(i,1,container);
            i++;
        }
    }

    // Set up parser engine with the new parser properties
    parserEngine->setVariables(parserWidget->variableList);
    QByteArray *check = new QByteArray;
    if(!parserEngine->isValid(check))
    {
        qDebug() << *check;
    }

}

void PlotterWidget::populatePlotArea()
{
    // Setup value vector
    valuesList.clear();

    // Add a layer for each number value
    customPlot->clearGraphs();
    foreach(ComplexVariable *var,*parserWidget->variableList)
    {
        if(var->type==NUMTYPE)
        {
            // Added number variable
            QVector<double> valvec;

            for(quint16 i=0;i<(quint16) xMax;i++)
            {
                valvec.append(0);
            }
            valuesList.append(valvec);

            // Add a graph for each number value
            customPlot->addGraph();
//            QBrush brush;
//            brush.setColor(QColor("black"));
            QPen pen;
            pen.setWidth(1);
            pen.setColor(QColor(255,0,0));
            //            customPlot->setBackground(brush);
            //            customPlot->setAutoFillBackground(true);
            customPlot->graph(customPlot->graphCount()-1)->setPen(pen);

//            QBrush brush = customPlot->graph(customPlot->graphCount()-1)->brush();
//            brush.setColor(QColor(100,100,100));
//            customPlot->graph(customPlot->graphCount()-1)->setBrush(brush);

            customPlot->setNoAntialiasingOnDrag(true);
            //            customPlot->graph(customPlot->graphCount()-1)setBackground(brush);
            //                        customPlot->graph(customPlot->graphCount()-1)->setAntialiased(true);
            //            customPlot->graph(customPlot->graphCount()-1)->setBrush(brush);
            //            customPlot->graph(customPlot->graphCount()-1)->setPen(pen);
            //            customPlot->graph(customPlot->graphCount()-1)->setBrush(QBrush("red",Qt::DashLine));


            customPlot->graph(customPlot->graphCount()-1)->setData(keys,valvec);
        }
    }
}

quint8 PlotterWidget::calcRowCount()
{
    // Calculate the total number of rows to use based on the total amount of variables.
    quint8 total=0;
    foreach(ComplexVariable *cv,*parserWidget->variableList)
    {
        if(cv->type==VECTYPE)
        {
            total+=cv->vector->size();
        }
        else
        {
            total++;
        }
    }
    //    qDebug() << total << " rows";
    return total;
}

void PlotterWidget::parsedDataReady(QList<RepeatedVector> parsedData)
{
    QString output;
    output.append("List found:\n");
    int complexCount=0;
    int numberCount=0;

    foreach(RepeatedVector repVector, parsedData)
    {
        QTableWidgetItem *item = tableWidget->item(complexCount,2);
        if(repVector.vectors.size()<2)
        {
            // Single variable
            if(repVector.vectors.at(0).vector.at(0)->varType==BYTTYPE)
            {
                item->setText(repVector.vectors.at(0).vector.at(0)->varBytes);
            }
            else
            {
                // Number variable
                double numVal = repVector.vectors.at(0).vector.at(0)->varValue;
                item->setText(QString("%1").arg(numVal));
                customPlot->graph(numberCount)->removeData(0,xMax);

                customPlot->graph(numberCount)->clearData();
                // Append data point to plot
                valuesList[numberCount].append(numVal);
                valuesList[numberCount].remove(0);
                if(tableWidget->item(complexCount,1)->checkState()==Qt::Checked)
                {
                    customPlot->graph(numberCount)->setData(keys,valuesList.at(numberCount));
                }

                //                customPlot->replot();
                numberCount++;
            }

        }
        else
        {
            // Vector
            item->setText("OK");
            //            tableWidget->setItem(i,1,"OK");

            //            foreach(SingleVector sinVector, repVector.vectors)
            //            {
            //                foreach(SingleResult *sinResult, sinVector.vector)
            //                {
            //                    switch(sinResult->varType)
            //                    {
            //                    case BYTTYPE:
            //                        output.append(" B: ");
            //                        output.append(sinResult->varBytes);
            //                        break;
            //                    default:
            //                        output.append(" N: ");
            //                        output.append(sinResult->varBytes);
            //                        output.append(QString(" (%1) ").arg(sinResult->varValue));
            //                        break;
            //                    }
            //                }
            //                output.append('\n');
            //            }
        }
        complexCount++;
    }

    //    qDebug() << output;
}

void PlotterWidget::adjustXRange()
{
    // Adjust the valuesList QList to reflect the new xMax value.
    // Two scenarios:
    // 1. The new xMax is larger than the existing valuesList QList
    // 2. The new xMax is smaller than the existing valueList QList

    if(valuesList.size()>0)
    {

        if((quint16)xMax > valuesList.at(0).size())
        {
            //            QVector<double> valvec;
//            qDebug() << "xMax (" << xMax << ") is larger than valuesList vector (" << valuesList.at(0).size() << ")";
            quint16 newZeroes = (quint16) xMax -valuesList.at(0).size();
//            qDebug() << "Prepend " << newZeroes << " zeroes";

            for(int i=0;i<valuesList.size();i++)
            {
                for(quint16 j=0;j<newZeroes;j++)
                {
                    valuesList[i].prepend(0);
                }
            }
        }
        else
        {
//            qDebug() << "xMax (" << xMax << ") is smaller than valuesList (" << valuesList.at(0).size() << ")";
            quint16 lessPoints = valuesList.at(0).size()- (quint16) xMax;
//            qDebug() << "Remove " << lessPoints << " points";
            for(int i=0;i<valuesList.size();i++)
            {
                    valuesList[i].remove(0,lessPoints);
            }

        }
    }
    //            if(xMax>valuesList.size())
    //            {
    //                for(int i=0;i<valuesList.size()-xMax;i++)
    //                {
    //                    valvec.append(0);
    //                }
    //                valuesList.prepend(valvec);
    //            }
    //            else
    //            {
    //                for(int i=0;i<xMax;i++)
    //                            {
    //                                valvec.append(0);
    //                            }
    //                valuesList.append(valvec);
    //            }
}


void PlotterWidget::setXRange(double newMax)
{
    xMax=newMax;

    //    connect(xRangeSpin,SIGNAL(valueChanged(double)),this,SLOT(setXRange(double)));
    //    xRangeSpin->setValue(xMax);
    //    connect(xRangeSpin,SIGNAL(valueChanged(double)),this,SLOT(setXRange(double)));

    keys.clear();
    for(int i=0;i<(quint16) xMax;i++)
    {
        keys.append(i);
    }
    customPlot->xAxis->setRange(0,xMax);
    adjustXRange();
}

void PlotterWidget::setYMin(QString newMin)
{
    yMin = newMin.toDouble();
    disconnect(yMinSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMin(QString)));
    yMinSpin->setValue(yMin);
    connect(yMinSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMin(QString)));
    customPlot->yAxis->setRangeLower(yMin);
    customPlot->replot();
}

void PlotterWidget::setYMax(QString newMax)
{
    yMax = newMax.toDouble();
    disconnect(yMaxSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMax(QString)));
    yMaxSpin->setValue(yMax);
    connect(yMaxSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMax(QString)));
    customPlot->yAxis->setRangeUpper(yMax);
    customPlot->replot();
}

//void tableItemClicked(QTableWidgetItem * item)
//{
//    // if check box has been clicked
//    if (m_pressedItemState != item->checkState())
//    {
//        // perform check logic here
//    }
//}

void PlotterWidget::setupUI()
{
    delIconPixmap = QPixmap(":/images/delete_icon.png");
    //    keys.append(0);
    //    keys = new QVector<double>;
    //     valuesList = new QList<QVector<double> *>;
    xMax=100;
    yMin = 0;
    yMax = 70;
    for(quint16 i=0;i<(quint16) xMax;i++)
    {
        keys.append(i);
    }

    connectionWidget = new ConnectionWidget;
    parserWidget = new ParserWidget;


    customPlot = new QCustomPlot;
    customPlot->xAxis->setRange(0,xMax);
    customPlot->yAxis->setRange(yMin,yMax);


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

//customPlot->xAxis->set

//    mygrid->setVisible(false);

    //    customPlot->setColor(QColor("black"));
    customPlot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    xRangeLabel = new QLabel("X Range");
    xRangeSpin = new QDoubleSpinBox;
    xRangeSpin->setDecimals(0);
    xRangeSpin->setValue(xMax);
    xRangeSpin->setMinimum(1);
    xRangeSpin->setMaximum(1000);

    yMinLabel = new QLabel("Y Min");
    yMinSpin = new QSpinBox;
    yMinSpin->setFixedHeight(24);
    yMinSpin->setValue(yMin);
    yMinSpin->setMinimum(-100000);
    yMinSpin->setMaximum(100000);

    yMaxLabel = new QLabel("Y Max");
    yMaxSpin = new QSpinBox;
    yMaxSpin->setFixedHeight(24);
    yMaxSpin->setValue(yMax);
    yMaxSpin->setMinimum(-100000);
    yMaxSpin->setMaximum(100000);

    connectionLabel = new QLabel("Connection");
    connectionLabel->setFixedHeight(24);

    connectionBox = new QComboBox;
    connectionBox->setItemDelegate(new QStyledItemDelegate);
    connectionBox->setFixedHeight(24);
    connectionBox->addItem("Connection 001");

    parserLabel = new QLabel("Parser");
    parserLabel->setFixedHeight(24);

    parserBox = new QComboBox;
    parserBox->setItemDelegate(new QStyledItemDelegate);
    parserBox->setFixedHeight(24);
    parserBox->addItem("Parser 001");

    removeButton = new QPushButton;
    removeButton->setFixedHeight(24);
    removeButton->setFixedWidth(24);
    removeButton->setIcon(QIcon(delIconPixmap));

    tableWidget = new QTableWidget(this);
    tableWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setColumnCount(3);
    QStringList tableHeaders;
    tableHeaders.append("Field");
    tableHeaders.append("Plot");
    tableHeaders.append("Value");
    tableWidget->setHorizontalHeaderLabels(tableHeaders);
    tableWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    QHeaderView *hv = tableWidget->horizontalHeader();
    hv->setStretchLastSection(true);
    hv->setSectionsClickable(false);
    hv->setFixedHeight(24);
    hv->setSectionResizeMode(1,QHeaderView::ResizeToContents);

    hv = tableWidget->verticalHeader();
    hv->setSectionsClickable(false);

    dataSourceLayout = new QHBoxLayout;
    dataSourceLayout->addWidget(connectionLabel);
    dataSourceLayout->addWidget(connectionBox);
    dataSourceLayout->addWidget(parserLabel);
    dataSourceLayout->addWidget(parserBox);
    dataSourceLayout->addWidget(removeButton);

    contentLayout = new QHBoxLayout;
    contentLayout->addWidget(tableWidget);
    contentLayout->addWidget(customPlot);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(dataSourceLayout);
    mainLayout->addLayout(contentLayout);

    mainLayout->addWidget(xRangeLabel);
    mainLayout->addWidget(xRangeSpin);
    mainLayout->addWidget(yMinLabel);
    mainLayout->addWidget(yMinSpin);
    mainLayout->addWidget(yMaxLabel);
    mainLayout->addWidget(yMaxSpin);

    this->setLayout(mainLayout);

    connect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));
    connect(parserBox,SIGNAL(activated(QString)),this,SLOT(changeParser(QString)));
    connect(removeButton,SIGNAL(clicked()),this,SIGNAL(removePlotter()));
    connect(xRangeSpin,SIGNAL(valueChanged(double)),this,SLOT(setXRange(double)));
    connect(yMinSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMin(QString)));
    connect(yMaxSpin,SIGNAL(valueChanged(QString)),this,SLOT(setYMax(QString)));
}
