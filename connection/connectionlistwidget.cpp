#include "connectionlistwidget.h"


ConnectionListWidget::ConnectionListWidget(QWidget *parent)
    : QWidget(parent)
{

    widgetNameLabel = new QLabel("Connection List");
    newConnBtn = new QPushButton("New Connection");
    newConnBtn->setFixedHeight(24);

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
    setFixedWidth(410);
    connect(newConnBtn,SIGNAL(clicked()),this,SLOT(newConnection()));
    connect(listWidget,SIGNAL(itemRemoved(int)),this,SLOT(itemRemoved(int)));
    connect(listWidget,SIGNAL(itemMoved(int, int, QListWidgetItem*)),this,SLOT(resorted(int,int,QListWidgetItem*)));
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

void ConnectionListWidget::resorted(int src, int dest, QListWidgetItem* item)
{
      connectionList.insert(dest, connectionList.takeAt(src));
}

void ConnectionListWidget::newConnection()
{
    ConnectionWidget *connWidget = new ConnectionWidget;
    connWidget->setName(newConnectionName());
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
    foreach(ConnectionWidget *connection,connectionList)
    {
        nameList.append(connection->getName());
    }
    emit connectionListChanged(nameList);
}

void ConnectionListWidget::connectionRemoved()
{
    ConnectionWidget* conn = static_cast<ConnectionWidget*>(QObject::sender());
     int row = connectionList.indexOf(conn);
     QListWidgetItem *item = listWidget->item(row);
     connectionList.removeAt(row);
     listWidget->removeItemWidget(item);
     listWidget->takeItem(row);

    // Check the remaining connection names
    checkAllNames();
    updateList();
}

QString ConnectionListWidget::newConnectionName()
{
    QString newName;
    quint16 nameCounter=1;
    bool match;
    if(connectionList.size()>0)
    {
        while(nameCounter<999)
        {
            match=false;
            newName=QString("Connection %1").arg(nameCounter);
            foreach(ConnectionWidget *conn,connectionList)
            {
                if(newName==conn->getName())
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
    quint16 index = connectionList.indexOf(conn);
    for(quint16 i=0;i<connectionList.size();i++)
    {
        if(i!=index)
        {
            if(conn->getName() == connectionList.at(i)->getName())
            {
                conn->setNameValid(false);
                return;
            }
        }
    }
    conn->setNameValid(true);

    checkAllNames();
    updateList();
}

void ConnectionListWidget::checkAllNames()
{
    bool valid;
    // Compare all connections to see if there are any we can validate
    for(quint16 i=0;i<connectionList.size();i++)
    {
        valid=true;
        if(!connectionList.at(i)->nameIsValid())
        {
            for(quint16 j=0;j<connectionList.size();j++)
            {
                if(j==i) break;
                if(connectionList.at(i)->getName() == connectionList.at(j)->getName())
                {
                    valid=false;
                }
            }
            connectionList.at(i)->setNameValid(valid);
        }
    }
}


