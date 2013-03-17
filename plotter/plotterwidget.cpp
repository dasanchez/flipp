#include "plotterwidget.h"


PlotterWidget::PlotterWidget(QWidget *parent)
    : QWidget(parent)
{
    connectionWidget = new ConnectionWidget;
    parserWidget = new ParserWidget;

    //    widgetNameLabel = new QLabel("Profiles");
    //    QFont font = widgetNameLabel->font();
    //    font.setPointSize(font.pointSize()+4);
    //    widgetNameLabel->setFont(font);

    //    newProfileButton = new QPushButton("New");
    //    newProfileButton->setFixedHeight(24);
    //    newProfileButton->setFixedWidth(100);

    connectionBox = new QComboBox;
    connectionBox->setItemDelegate(new QStyledItemDelegate);
    connectionBox->setFixedHeight(24);
    connectionBox->addItem("Connection 001");

    parserBox = new QComboBox;
    parserBox->setItemDelegate(new QStyledItemDelegate);
    parserBox->setFixedHeight(24);
    parserBox->addItem("Parser 001");

    tableWidget = new QTableWidget(this);
    tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setColumnCount(2);
    QStringList tableHeaders;
    tableHeaders.append("Field");
    tableHeaders.append("Value");
    tableWidget->setHorizontalHeaderLabels(tableHeaders);
    tableWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
    //    tableWidget->setMaximumWidth(300);

    QHeaderView *hv = tableWidget->horizontalHeader();
    hv->setStretchLastSection(true);
    hv->setSectionResizeMode(1,QHeaderView::Fixed);
    hv->setSectionsClickable(false);
    hv->setFixedHeight(24);


    //    hv->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    hv = tableWidget->verticalHeader();
    hv->setSectionsClickable(false);
    //    hv->setSectionResizeMode(QHeaderView::Fixed);


    plotFrame = new QFrame;
    plotFrame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //    topLayout = new QHBoxLayout;
    //    topLayout->addWidget(widgetNameLabel);
    //    topLayout->addWidget(newProfileButton);

    dataSourceLayout = new QHBoxLayout;
    dataSourceLayout->addWidget(connectionBox);
    dataSourceLayout->addWidget(parserBox);

    contentLayout = new QHBoxLayout;
    contentLayout->addWidget(tableWidget);
    contentLayout->addWidget(plotFrame);


    mainLayout = new QVBoxLayout(this);
    //    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(dataSourceLayout);
    //    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(contentLayout);

    this->setLayout(mainLayout);

    connect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));
    connect(parserBox,SIGNAL(activated(QString)),this,SLOT(changeParser(QString)));

    //    setFixedWidth(410);

}

PlotterWidget::~PlotterWidget()
{

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
    //    connect(connectionWidget,SIGNAL(dataRx(QByteArray)),this,SLOT(dataReceived(QByteArray)));
    //    connect(this,SIGNAL(sendData(QByteArray)),connectionWidget,SLOT(dataTx(QByteArray)));
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
    connect(parserWidget,SIGNAL(deleteParser()),this,SLOT(detachParser()));
    // Populate QTableWidget
    populateParserTable();
}

void PlotterWidget::detachParser()
{
    parserWidget = new ParserWidget;
}

void PlotterWidget::populateParserTable()
{
    qDebug() << "populate table";
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
                tableWidget->setItem(i,1,item2);
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
            i++;
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
