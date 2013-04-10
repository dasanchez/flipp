#include "plotterwidget.h"

PlotterWidget::PlotterWidget(QWidget *parent)
    : QWidget(parent)
{
    delIconPixmap = QPixmap(":/images/delete_icon.png");
    //    keys.append(0);
    //    keys = new QVector<double>;
    //     valuesList = new QList<QVector<double> *>;
    for(quint8 i=0;i<100;i++)
    {
        keys.append(i);
    }

    connectionWidget = new ConnectionWidget;
    parserWidget = new ParserWidget;
    parserEngine = new ParserEngine;

    customPlot = new QCustomPlot;
    customPlot->xAxis->setRange(0,100);
    customPlot->yAxis->setRange(41985,42015);
    customPlot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    connectionBox = new QComboBox;
    connectionBox->setItemDelegate(new QStyledItemDelegate);
    connectionBox->setFixedHeight(24);
    connectionBox->addItem("Connection 001");

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
    dataSourceLayout->addWidget(connectionBox);
    dataSourceLayout->addWidget(parserBox);
    dataSourceLayout->addWidget(removeButton);

    contentLayout = new QHBoxLayout;
    contentLayout->addWidget(tableWidget);
    contentLayout->addWidget(customPlot);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(dataSourceLayout);
    mainLayout->addLayout(contentLayout);

    this->setLayout(mainLayout);

    connect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));
    connect(parserBox,SIGNAL(activated(QString)),this,SLOT(changeParser(QString)));
    connect(removeButton,SIGNAL(clicked()),this,SIGNAL(removePlotter()));
    connect(parserEngine,SIGNAL(dataParsed(QList<RepeatedVector>)),this,SLOT(parsedDataReady(QList<RepeatedVector>)));

    //    setFixedWidth(410);

}

PlotterWidget::~PlotterWidget()
{

    //    keys.clear();
    //    delete keys;
    //customPlot->deleteLater();
}

void PlotterWidget::updateConnections(QStringList *connectionNames)
{
    connectionBox->clear();
    connectionBox->addItems(*connectionNames);
    //    int index = connectionBox->findText(connectionWidget->getName());
    //    if(index>=0)
    //        connectionBox->setCurrentIndex(index);
    changeConnection(connectionBox->currentText());
}

void PlotterWidget::updateParsers(QStringList *parserNames)
{
    parserBox->clear();
    parserBox->addItems(*parserNames);
    //    int index = parserBox->findText(connectionWidget->getName());
    //    if(index>=0)
    //        parserBox->setCurrentIndex(index);
}

void PlotterWidget::changeConnection(QString connection)
{
    //    disconnect(connectionWidget,SIGNAL(dataRx(QByteArray)),this,SLOT(dataReceived(QByteArray)));
    //    disconnect(this,SIGNAL(sendData(QByteArray)),connectionWidget,SLOT(dataTx(QByteArray)));
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
            //                        customPlot->addGraph();
            QVector<double> valvec;
            for(quint8 i=0;i<100;i++)
            {
                valvec.append(0);
            }
            valuesList.append(valvec);
            // Add a graph for each number value
            customPlot->addGraph();


            //            customPlot->graph(customPlot->graphCount()-1)->setData(keys,valvec);
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
    qDebug() << total << " rows";
    return total;
}

void PlotterWidget::parsedDataReady(QList<RepeatedVector> parsedData)
{
    QString output;
    output.append("List found:\n");
    int i=0;
    int numberCount=0;

    foreach(RepeatedVector repVector, parsedData)
    {
        QTableWidgetItem *item = tableWidget->item(i,2);
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

                // Append data point to plot
                valuesList[numberCount].append(numVal);
                valuesList[numberCount].remove(0);
                customPlot->graph(numberCount)->setData(keys,valuesList.at(numberCount));
                customPlot->replot();
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
        i++;
    }

    //    qDebug() << output;
}
