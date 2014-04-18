#include "datastagingwidget.h"

DataStagingWidget::DataStagingWidget(QWidget *parent) :
    QWidget(parent)
{
    parserEngine = new ParserEngine;

    widgetNameLabel = new QLabel("Data Staging");
    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);
    newVariableButton = new QPushButton("New Variable");
    newVariableButton->setFixedHeight(24);
    newVariableButton->setFixedWidth(100);

    connectionBox = new QComboBox;
    parserBox = new QComboBox;
    parserWidget = new ParserWidget;
    parserEngine = new ParserEngine;
    connectionWidget = new ConnectionWidget;

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(3);
    QStringList tableHeaders;
    tableHeaders.append("Variable");
    tableHeaders.append("Value");
    tableHeaders.append("Plot");
    tableWidget->setHorizontalHeaderLabels(tableHeaders);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    //    QHeaderView *hv = tableWidget->horizontalHeader();
    //    hv->setStretchLastSection(true);
    //    hv->setSectionsClickable(false);
    //    hv->setFixedHeight(24);
    //    hv->setSectionResizeMode(1,QHeaderView::ResizeToContents);

    //    hv = tableWidget->verticalHeader();
    //    hv->setSectionsClickable(false);


    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newVariableButton);

    dataSourceLayout = new QHBoxLayout;
    dataSourceLayout->addWidget(connectionBox);
    dataSourceLayout->addWidget(parserBox);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(dataSourceLayout);
    mainLayout->addWidget(tableWidget);
    //    mainLayout->addWidget(scrollArea);
    this->setLayout(mainLayout);

    setMinimumWidth(400);
    setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    connect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));
    connect(parserBox,SIGNAL(activated(QString)),this,SLOT(changeParser(QString)));
    connect(parserEngine,SIGNAL(dataParsed(QList<RepeatedVector>)),this,SLOT(parsedDataReady(QList<RepeatedVector>)));
}

DataStagingWidget::~DataStagingWidget()
{

}

void DataStagingWidget::updateConnections(QStringList connectionNames)
{
    connectionBox->clear();
    connectionBox->addItems(connectionNames);
    //    int index = connectionBox->findText(connectionWidget->getName());
    //  if(index>=0)
    //      connectionBox->setCurrentIndex(index);
    //  changeConnection(connectionBox->currentText());

}

void DataStagingWidget::updateParsers(QStringList parserNames)
{
    parserBox->clear();
    parserBox->addItems(parserNames);
}

void DataStagingWidget::changeConnection(QString connection)
{
    emit stagingConnectionRequest(connection);
}

void DataStagingWidget::assignConnection(ConnectionWidget *connWidget)
{
    connectionWidget=connWidget;
    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(connectionWidget,SIGNAL(widgetRemoved()),this,SLOT(detachConnection()));
}

void DataStagingWidget::detachConnection()
{
    connectionWidget = new ConnectionWidget;
}

void DataStagingWidget::changeParser(QString parserName)
{

    emit stagingParserRequest(parserName);
}

void DataStagingWidget::assignParser(ParserWidget *parser)
{
    parserWidget = parser;

    connect(parserWidget,SIGNAL(updateVariableList()),this,SLOT(populateParserTable()));
    connect(parserWidget,SIGNAL(deleteParser()),this,SLOT(detachParser()));

    // Populate QTableWidget and QCustomPlot
    populateParserTable();

}

void DataStagingWidget::detachParser()
{
        parserWidget = new ParserWidget;
}

void DataStagingWidget::populateParserTable()
{
    quint8 i=0;
    tableWidget->setRowCount(calcRowCount());
    foreach(ComplexVariable *var,*parserWidget->variableList)
    {
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
                tableWidget->setItem(i,1,item2);
                // Checkbox
//                QWidget *container = new QWidget;
//                QHBoxLayout *layout = new QHBoxLayout;
//                layout->setSpacing(0);
//                layout->setMargin(0);
//                QCheckBox *checkBox = new QCheckBox;
//                layout->addStretch(1);
//                layout->addWidget(checkBox);
//                layout->addStretch(1);
//                container->setLayout(layout);
//                tableWidget->setCellWidget(i,1,container);
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
            tableWidget->setItem(i,1,item2);
            // Checkbox
//            QTableWidgetItem *item3 = new QTableWidgetItem;
//            item3->setCheckState(Qt::Unchecked);
//            tableWidget->setItem(i,1,item3);
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

quint8 DataStagingWidget::calcRowCount()
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

void DataStagingWidget::parsedDataReady(QList<RepeatedVector> parsedData)
{
    QString output;
    output.append("List found:\n");
    int complexCount=0;
    int numberCount=0;

    foreach(RepeatedVector repVector, parsedData)
    {
        QTableWidgetItem *item = tableWidget->item(complexCount,1);
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
//                if(tableWidget->item(complexCount,1)->checkState()==Qt::Checked)
//                {
//                    customPlot->graph(numberCount)->addData(key,numVal);

//                    while(customPlot->graph(numberCount)->data()->size()>xMax)
//                    {
//                        customPlot->graph(numberCount)->removeData(customPlot->graph(numberCount)->data()->keys().at(0));
//                    }

//                    if(yAxisAutoRange)
//                    {
//                        customPlot->graph(numberCount)->rescaleValueAxis();
//                    }

//                    // make key axis range scroll with the data:

//                    customPlot->xAxis->setRange(customPlot->graph(numberCount)->data()->keys().at(0),key+0.01);

//                }
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
