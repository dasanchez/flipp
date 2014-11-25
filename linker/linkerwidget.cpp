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
    tableWidget->setColumnCount(2);
    QStringList tableHeaders;
    tableHeaders.append("Variable");
    tableHeaders.append("Value");
    tableWidget->setHorizontalHeaderLabels(tableHeaders);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    boxList = new QList<QCheckBox*>;

    QHeaderView *hv = tableWidget->horizontalHeader();

    hv->setSectionsClickable(false);

    dataSourceLayout = new QHBoxLayout;
    dataSourceLayout->addWidget(connectionBox);
    dataSourceLayout->addWidget(parserBox);
    dataSourceLayout->addWidget(removeButton);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(dataSourceLayout);
    mainLayout->addWidget(tableWidget);

    this->setLayout(mainLayout);

    setMinimumWidth(100);

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    parserEngine->moveToThread(thread);

    connect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));
    connect(parserBox,SIGNAL(activated(QString)),this,SLOT(changeParser(QString)));
    connect(removeButton,SIGNAL(clicked()),this,SIGNAL(removeLinker()));
    connect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
    connect(thread,SIGNAL(started()),this,SLOT(threadStarted()));
    thread->start();
}

LinkerWidget::~LinkerWidget()
{

}

void LinkerWidget::threadStarted()
{
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
    if(parserBox->count()==1)
    {
        emit linkerParserRequest(parserBox->currentText());
    }
}

QString LinkerWidget::getConnection()
{
    return connectionBox->currentText();
}

QString LinkerWidget::getParser()
{
    return parserBox->currentText();
}

void LinkerWidget::changeConnection(QString connection)
{
    emit linkerConnectionRequest(connection);
}


void LinkerWidget::assignConnection(ConnectionWidget *connWidget)
{
    connectionWidget=connWidget;
    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(connectionWidget,SIGNAL(widgetRemoved()),this,SLOT(detachConnection()));
}

void LinkerWidget::detachConnection()
{
    connectionWidget = new ConnectionWidget;
}

void LinkerWidget::changeParser(QString parserName)
{
    parserEngine->setParser(false);
    disconnect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
    disconnect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    emit linkerParserRequest(parserName);
}

void LinkerWidget::assignParser(ParserWidget *parser)
{
    variables = parser->variableList;
    results.clear();
    foreach(ComplexVariable cv,variables)
    {
        ParsedVariable i;
        i.type=cv.type;
        results.append(i);
    }
    parserEngine->setVariables(parser->variableList);
    populateParserTable();
    connect(parser,SIGNAL(updateVariableList(QList<ComplexVariable>)),this,SLOT(newParserVariables(QList<ComplexVariable>)));
    connect(parser,SIGNAL(deleteParser()),this,SLOT(detachParser()));
    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
    parserEngine->setParser(true);
}

void LinkerWidget::newParserVariables(QList<ComplexVariable> newVars)
{    
    variables = newVars;
    results.clear();
    foreach(ComplexVariable cv,newVars)
    {
        ParsedVariable i;
        i.type=cv.type;
        results.append(i);
    }

    disconnect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    disconnect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
    parserEngine->setVariables(newVars);
    parserEngine->clearVariables();
    populateParserTable();
    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
}

void LinkerWidget::detachParser()
{
    if(parserBox->count()>0)
    {
        emit linkerParserRequest(parserBox->currentText());
    }
    else
    {
        disconnect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
        QList<ComplexVariable> emptyList;
        parserEngine->setVariables(emptyList);
        connect(connectionWidget,SIGNAL(dataRx(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    }

}

void LinkerWidget::populateParserTable()
{
    quint8 i=0;
    boxList->clear();

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
                    i++;

                }
            }
            else
            {

                QTableWidgetItem *item = new QTableWidgetItem(var.name);
                item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
                item->setFlags(Qt::NoItemFlags);
                tableWidget->setItem(i,0,item);
                QTableWidgetItem *item2 = new QTableWidgetItem("-");
                item2->setTextAlignment(Qt::AlignBottom);
                item2->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
                item2->setFlags(Qt::NoItemFlags);
                tableWidget->setItem(i,1,item2);


                i++;
            }
        }

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
    return total;
}

void LinkerWidget::parsedDataReady(VariableList parsedData)
{
    int complexCount=0;

    if(parsedData.size()==tableWidget->rowCount())
    {
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
                    results[complexCount].content = repVector.vectors.at(0).vector.at(0).varBytes;
                    results[complexCount].value = repVector.vectors.at(0).vector.at(0).varValue;
                }

            }
//            else
//            {
//                // Vector
//                item->setText("OK");
//            }
            complexCount++;
        }
        emit newDataPoint();
    }
}

