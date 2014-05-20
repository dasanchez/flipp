#include "linkerwidget.h"

LinkerWidget::LinkerWidget(QWidget *parent) :
    QWidget(parent)
{
    parserEngine = new ParserEngine;
    thread = new QThread;

    connectionBox = new QComboBox;
    parserBox = new QComboBox;
    removeButton = new QPushButton("Remove");
    removeButton->setFixedHeight(24);

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
    tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);



    QHeaderView *hv = tableWidget->horizontalHeader();

    hv->setSectionsClickable(false);
    hv->setSectionResizeMode(2,QHeaderView::Fixed);

    dataSourceLayout = new QHBoxLayout;
    dataSourceLayout->addWidget(connectionBox);
    dataSourceLayout->addWidget(parserBox);
    dataSourceLayout->addWidget(removeButton);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(dataSourceLayout);
    mainLayout->addWidget(tableWidget);

    this->setLayout(mainLayout);

    setMinimumWidth(200);

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    parserEngine->moveToThread(thread);

    connect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));
    connect(parserBox,SIGNAL(activated(QString)),this,SLOT(changeParser(QString)));
    connect(removeButton,SIGNAL(clicked()),this,SIGNAL(removeLinker()));
    connect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
    connect(parserEngine,SIGNAL(intOut(int)),this,SLOT(testThread(int)));
    connect(thread,SIGNAL(started()),this,SLOT(threadStarted()));
    thread->start();
}

LinkerWidget::~LinkerWidget()
{

}

void LinkerWidget::threadStarted()
{
    //    qDebug() << "Thread started";
}

void LinkerWidget::updateConnections(QStringList connectionNames)
{
    connectionBox->clear();
    connectionBox->addItems(connectionNames);
}

void LinkerWidget::updateParsers(QStringList parserNames)
{
    parserBox->clear();
    parserBox->addItems(parserNames);
}

void LinkerWidget::changeConnection(QString connection)
{
    emit linkerConnectionRequest(connection);
}

void LinkerWidget::assignConnection(ConnectionWidget *connWidget)
{
    connectionWidget=connWidget;
    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    //    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(newData(QByteArray)));
    //    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),thread,SLOT(start()));
    connect(connectionWidget,SIGNAL(widgetRemoved()),this,SLOT(detachConnection()));
}

void LinkerWidget::detachConnection()
{
    connectionWidget = new ConnectionWidget;
}

void LinkerWidget::changeParser(QString parserName)
{

    emit linkerParserRequest(parserName);
}

void LinkerWidget::assignParser(ParserWidget *parser)
{
    //    parserWidget = parser;

    //    parserEngine->clearVariables();

    disconnect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));

    parserEngine->setVariables(parser->variableList);


    connect(parser,SIGNAL(updateVariableList(QList<ComplexVariable>)),this,SLOT(newParserVariables(QList<ComplexVariable>)));
//    connect(parser,SIGNAL(deleteParser()),this,SLOT(detachParser()));

    populateParserTable();
    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));

}

void LinkerWidget::newParserVariables(QList<ComplexVariable> newVars)
{
    disconnect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    parserEngine->setVariables(newVars);
    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    populateParserTable();
}

void LinkerWidget::detachParser()
{
//    qDebug() << "Clearing variables";
//    disconnect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
//    parserEngine = new ParserEngine;

//    disconnect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
  //  QList<ComplexVariable> emptyList;

    //parserEngine->setVariables(emptyList);
//    populateParserTable();
//    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    //    parserWidget = new ParserWidget;
}

void LinkerWidget::populateParserTable()
{
    quint8 i=0;

    tableWidget->setRowCount(calcRowCount());
    if(parserEngine->getVariables().size()>0)
    {

        foreach(ComplexVariable var, parserEngine->getVariables())
        {
            if(var.type==VECTYPE)
            {
                QString fullName;
                foreach(BaseVariable bvar, var.vector)
                {
                    fullName.clear();
                    fullName.append(var.name);
                    fullName.append(".");
                    fullName.append(bvar.name);
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
                    //                                QWidget *container = new QWidget;
                    //                                QHBoxLayout *layout = new QHBoxLayout;
                    //                                layout->setSpacing(0);
                    //                                layout->setMargin(0);
                    //                                QCheckBox *checkBox = new QCheckBox;
                    //                                layout->addStretch(1);
                    //                                layout->addWidget(checkBox);
                    //                                layout->addStretch(1);
                    //                                container->setLayout(layout);
                    //                                tableWidget->setCellWidget(i,1,container);
                    i++;

                }
            }
            else
            {
                //            qDebug() << var.name;
                QTableWidgetItem *item = new QTableWidgetItem(var.name);
                item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
                item->setFlags(Qt::NoItemFlags);
                tableWidget->setItem(i,0,item);
                QTableWidgetItem *item2 = new QTableWidgetItem("-");
                item2->setTextAlignment(Qt::AlignBottom);
                item2->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
                item2->setFlags(Qt::NoItemFlags);
                tableWidget->setItem(i,1,item2);
                // Checkbox
                //            QTableWidgetItem *cbItem = new QTableWidgetItem;
                //            cbItem->setCheckState(Qt::Unchecked);
                //            cbItem->setTextAlignment(Qt::AlignRight);
                //            tableWidget->setItem(i,2,cbItem);
                // v2:
                QWidget *container = new QWidget;
                QHBoxLayout *layout = new QHBoxLayout;
                layout->setSpacing(0);
                layout->setMargin(0);
                QCheckBox *checkBox = new QCheckBox;
                layout->addStretch(1);
                layout->addWidget(checkBox);
                layout->addStretch(1);
                container->setLayout(layout);
                tableWidget->setCellWidget(i,2,container);


                i++;
            }
        }
        tableWidget->setColumnWidth(2,50);
        //    tableWidget->set

        // Set up parser engine with the new parser properties
        // parserEngine->setVariables(parserEngine->getVariables());
        QByteArray *check = new QByteArray;
        if(!parserEngine->isValid(check))
        {

            qDebug() << *check;
        }
    }
}

quint8 LinkerWidget::calcRowCount()
{
    // Calculate the total number of rows to use based on the total amount of variables.
    quint8 total=0;
    foreach(ComplexVariable cv, parserEngine->getVariables())
    {
        if(cv.type==VECTYPE)
        {
            total+=cv.vector.size();
        }
        else
        {
            total++;
        }
    }
    //    qDebug() << total << " rows";
    return total;
}

void LinkerWidget::testThread(int value)
{
    qDebug() << value;
}

void LinkerWidget::parsedDataReady(VariableList parsedData)
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
            if(repVector.vectors.at(0).vector.at(0).varType==BYTTYPE)
            {
                item->setText(repVector.vectors.at(0).vector.at(0).varBytes);
            }
            else
            {
                // Number variable
                double numVal = repVector.vectors.at(0).vector.at(0).varValue;
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

