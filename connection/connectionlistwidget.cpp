#include "connectionlistwidget.h"


ConnectionListWidget::ConnectionListWidget(QWidget *parent, QList<ConnectionUnit*> *conns)
    : QWidget(parent)
{
    //    nameList = new QStringList;
    connections = conns;
    widgetNameLabel = new QLabel("Connections");
    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);
    newConnBtn = new QPushButton("New");
    newConnBtn->setFont(font);
    newConnBtn->setFixedWidth(90);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newConnBtn);

    listWidget = new LiveListWidget(this);
    listWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(listWidget);

    this->setLayout(mainLayout);
    setFixedWidth(550);
    connect(newConnBtn,SIGNAL(clicked()),this,SLOT(newConnection()));
    connect(listWidget,SIGNAL(itemRemoved(int)),this,SLOT(itemRemoved(int)));
    connect(listWidget,SIGNAL(itemMoved(int, int, QListWidgetItem*)),this,SLOT(resorted(int,int)));
}

ConnectionListWidget::~ConnectionListWidget()
{

}

void ConnectionListWidget::sizeChanged(QSize newSize)
{
    ConnectionWidget* conn = static_cast<ConnectionWidget*>(QObject::sender());
    int row = connectionList.indexOf(conn);
    listWidget->item(row)->setSizeHint(newSize);
}

void ConnectionListWidget::itemRemoved(int row)
{
    connectionList.removeAt(row);
}

void ConnectionListWidget::resorted(int src, int dest)
{
    connectionList.insert(dest, connectionList.takeAt(src));
}

void ConnectionListWidget::newConnection()
{
    ConnectionUnit *connectionUnit = new ConnectionUnit;
    connectionUnit->setName(newConnectionName());
    connections->append(connectionUnit);

    ConnectionWidget *connWidget = new ConnectionWidget(this, connectionUnit);
    connectionList.append(connWidget);

    QListWidgetItem *item = new QListWidgetItem;
    listWidget->addItem(item);
    listWidget->setItemWidget(item,connWidget);
    item->setSizeHint(connWidget->sizeHint());

    connect(connWidget,SIGNAL(nameChange()),this,SLOT(nameChanged()));
    connect(connWidget,SIGNAL(widgetRemoved()),this,SLOT(connectionRemoved()));
    connect(connWidget,SIGNAL(sizeChange(QSize)),this,SLOT(sizeChanged(QSize)));
    updateList();
}

void ConnectionListWidget::addConnection(ConnectionUnit *cUnit)
{
    ConnectionWidget *cw = new ConnectionWidget(this,cUnit);
    connectionList.append(cw);
    QListWidgetItem *item = new QListWidgetItem;
    listWidget->addItem(item);
    listWidget->setItemWidget(item,cw);
    item->setSizeHint(cw->sizeHint());

    connect(cw,SIGNAL(nameChange()),this,SLOT(nameChanged()));
    connect(cw,SIGNAL(widgetRemoved()),this,SLOT(connectionRemoved()));
    connect(cw,SIGNAL(sizeChange(QSize)),this,SLOT(sizeChanged(QSize)));
    updateList();
}

void ConnectionListWidget::addConnection(QString name, int connType, QString address, QString port)
{
    ConnectionUnit *cUnit  = new ConnectionUnit;
    cUnit->setName(name);
    cUnit->setType((connectionType) connType);
    cUnit->setAddress_Port(address);
    cUnit->setPort_Baud(port.toLong());
    connections->append(cUnit);

    ConnectionWidget *connWidget = new ConnectionWidget(this,cUnit);
    connectionList.append(connWidget);

    QListWidgetItem *item = new QListWidgetItem;
    listWidget->addItem(item);
    listWidget->setItemWidget(item,connWidget);
    item->setSizeHint(connWidget->sizeHint());

    connect(connWidget,SIGNAL(nameChange()),this,SLOT(nameChanged()));
    connect(connWidget,SIGNAL(widgetRemoved()),this,SLOT(connectionRemoved()));
    connect(connWidget,SIGNAL(sizeChange(QSize)),this,SLOT(sizeChanged(QSize)));
    updateList();
}


void ConnectionListWidget::updateList()
{
    nameList.clear();
    for(quint16 i=0;i<connections->size();i++)
    {
        nameList.append(connections->at(i)->getName());
    }

    emit connectionListChanged(nameList);
}

void ConnectionListWidget::connectionRemoved()
{
    ConnectionWidget* conn = static_cast<ConnectionWidget*>(QObject::sender());
    connections->removeAt(connections->indexOf(conn->connectionUnit));

    int row = connectionList.indexOf(conn);
    QListWidgetItem *item = listWidget->item(row);
    connectionList.removeAt(row);
    listWidget->removeItemWidget(item);
    listWidget->takeItem(row);

    // Check the remaining connection names
    checkAllNames();
    updateList();
}

// Generate a valid name for a new connection. //
QString ConnectionListWidget::newConnectionName()
{
    QString newName;
    quint16 nameCounter=1;
    bool match;
    if(connections->size()>0)
    {
        while(nameCounter<999)
        {
            match=false;
            newName=QString("Connection %1").arg(nameCounter);
            for(quint16 i=0;i<connections->size();i++)
            {
                if(newName == connections->at(i)->getName())
                {
                    match=true;
                    nameCounter++;
                    break;
                }
            }
            if(!match)
            {
                return newName;
            }
        }
        return QString("");

    }
    return QString("Connection 1");
}

void ConnectionListWidget::nameChanged()
{
    ConnectionWidget* conn = qobject_cast<ConnectionWidget *>(QObject::sender());
    // Compare sender string against other connections
    quint16 index = connections->indexOf(conn->connectionUnit);
    for(quint16 i=0;i<connections->size();i++)
    {
        if(i!=index)
        {
            if(conn->connectionUnit->getName() == connections->at(i)->getName())
            {
                conn->connectionUnit->setInvalid();
                return;
            }
        }
    }
    conn->connectionUnit->setValid();

    checkAllNames();
    updateList();
}

void ConnectionListWidget::checkAllNames()
{
    bool valid;
    // Compare all connections to see if there are any we can validate
    for(quint16 i=0;i<connections->size();i++)
    {
        valid=true;
        if(!connections->at(i)->isValid())
        {
            for(quint16 j=0;j<connections->size();j++)
            {
                if(j==i) break;
                if(connections->at(i)->getName() == connections->at(j)->getName())
                {
                    valid=false;
                }
            }
            valid ? connections->at(i)->setValid() : connections->at(i)->setInvalid();
        }
    }
}


