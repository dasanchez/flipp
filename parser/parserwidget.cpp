#include "parserwidget.h"
#include <QDebug>
#include <QDir>

ParserWidget::ParserWidget(QWidget *parent) :
    QWidget(parent)
{
    parserUnit = new ParserUnit;
    setupUI();

    // Connect ParserUnit signals
}

ParserWidget::ParserWidget(QWidget *parent, ParserUnit *pUnit) :
    QWidget(parent),
    parserUnit(pUnit)
{
    setupUI_fromParser();

    // Populate variable widgets
//    qDebug() << parserUnit->variableList.size() << " variables in this parser";

    foreach(ComplexVariable cVar,parserUnit->variableList)
    {
        // Generate variable widget
        VariableWidget *vWidget = new VariableWidget(this,cVar);
        addVariableWidget(vWidget);
    }

    // Connect ParserUnit signals
}


ParserWidget::~ParserWidget()
{
    //    delete ui;
}

//QString ParserWidget::getName()
//{
//    return nameEdit->text();
//}

//bool ParserWidget::hasValidName()
//{
//    return validName;
//}

void ParserWidget::setNameValid(bool isValid)
{
    validName = isValid;
    QByteArray *ba = new QByteArray;
    if(validName && listIsValid(variableList,ba))
    {
        statusBar->setText("Ready");
    }
    else
    {
        if(validName)
        {
         statusBar->setText(*ba);
        }
        else
        {
        statusBar->setText("Name is not valid");
        }
    }
}

//void ParserWidget::setName(QString newName)
//{
//    disconnect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
//    nameEdit->setText(newName);
//    connect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
//}


void ParserWidget::toggleExpand()
{
    expanded=!expanded;
    if(expanded)
    {
        newItemLayout->addWidget(addByteButton);
        newItemLayout->addWidget(addNumberButton);
        newItemLayout->addWidget(addVectorButton);
        mainLayout->addWidget(liveListWidget);
        addByteButton->setVisible(true);
        addNumberButton->setVisible(true);
        addVectorButton->setVisible(true);
        liveListWidget->setVisible(true);
        expandButton->setText("Less");
    }
    else
    {
        newItemLayout->removeWidget(addByteButton);
        newItemLayout->removeWidget(addNumberButton);
        newItemLayout->removeWidget(addVectorButton);
        mainLayout->removeWidget(liveListWidget);
        addByteButton->setVisible(false);
        addNumberButton->setVisible(false);
        addVectorButton->setVisible(false);
        liveListWidget->setVisible(false);
        expandButton->setText("More");
    }
    emit update();
    emit changeSize(this->sizeHint());
}

void ParserWidget::variableListChanged()
{
    for(int i=0;i<variableList.size();i++)
    {
        variableList[i]=vwList->at(i)->variable;
    }

    QByteArray *validResponse = new QByteArray;
    listIsValid(variableList, validResponse);
    statusBar->setText(*validResponse);

    printList();
    emit updateVariableList(variableList);
}

void ParserWidget::newVariable()
{
    QPushButton *senderButton = static_cast<QPushButton*>(QObject::sender());
    VariableWidget *vw = new VariableWidget(liveListWidget);
    if(senderButton==addNumberButton)
    {
        vw->setNumber();
    }
    else if(senderButton==addVectorButton)
    {
        vw->setVector();
    }

    vwList->append(vw);
    variableList.append(vw->variable);
    QListWidgetItem *item = new QListWidgetItem(liveListWidget);

    liveListWidget->addItem(item);
    item->setSizeHint(vw->sizeHint());
    liveListWidget->setItemWidget(item,vw);

    connect(vw,SIGNAL(sizeToggled(QSize)),this,SLOT(itemSize(QSize)));
    connect(vw,SIGNAL(deleteVar()),this,SLOT(remVariable()));
    connect(vw,SIGNAL(variableChanged()),this,SLOT(variableListChanged()));
    variableListChanged();
}


void ParserWidget::addVariableWidget(VariableWidget *vw)
{
    vwList->append(vw);
    variableList.append(vw->variable);
    QListWidgetItem *item = new QListWidgetItem(liveListWidget);
    liveListWidget->addItem(item);
    item->setSizeHint(vw->sizeHint());
    liveListWidget->setItemWidget(item,vw);

    connect(vw,SIGNAL(sizeToggled(QSize)),this,SLOT(itemSize(QSize)));
    connect(vw,SIGNAL(deleteVar()),this,SLOT(remVariable()));
    connect(vw,SIGNAL(variableChanged()),this,SLOT(variableListChanged()));
    variableListChanged();
}

void ParserWidget::remVariable()
{
    VariableWidget *vw = static_cast<VariableWidget*>(QObject::sender());
    int row = vwList->indexOf(vw);
    QListWidgetItem *item = liveListWidget->item(row);
    variableList.removeAt(row);
    liveListWidget->removeItemWidget(item);
    liveListWidget->takeItem(row);
    vwList->removeAt(row);
    delete vw;
    variableListChanged();
}

void ParserWidget::itemSize(QSize newSize)
{
    VariableWidget *vw = static_cast<VariableWidget*>(QObject::sender());
    int row = vwList->indexOf(vw);
    QListWidgetItem *item = liveListWidget->item(row);
    item->setSizeHint(newSize);
}

void ParserWidget::resorted(int src,int dest,QListWidgetItem* item)
{
    // Resort in list:
    vwList->insert(dest, vwList->takeAt(src));
    variableList.insert(dest,variableList.takeAt(src));
    variableListChanged();
}

void ParserWidget::itemRemoved(int row)
{
    variableList.removeAt(row);
    delete vwList->at(row);
    vwList->removeAt(row);
    variableListChanged();
}

void ParserWidget::printList()
{
    foreach(ComplexVariable item, variableList)
    {
        QString outString = item.name;
        outString.append("| type: ");
        switch(item.type)
        {
        case BYTTYPE:
            outString.append("byt, ");
            break;
        case NUMTYPE:
            outString.append("num, ");
            break;
        default:
            outString.append("vec, rep: ");
            outString.append(QString("%1:\n").arg(item.repeat));
            // Print vector contents
            foreach(BaseVariable vecItem, item.vector)
            {
                outString.append("\t");
                outString.append(vecItem.name);
                outString.append("| ");
                // Type
                if(vecItem.type==BYTTYPE)
                {
                    outString.append("byt, ");
                }
                else
                {
                    outString.append("num, ");
                }
                // Length
                if(vecItem.fixed)
                {
                    outString.append("length: fix, ");
                    outString.append(QString("len_val: %1, ").arg(vecItem.length));
                }
                else
                {
                    outString.append("length: var, ");
                }
                // Match
                if(vecItem.match)
                {
                    outString.append("match: yes: , ");
                    outString.append(vecItem.matchBytes.toHex());
                }
                else
                {
                    outString.append("match: no");
                }
                outString.append("\n");
            }

            break;
        }
        if(item.type!=VECTYPE)
        {
            if(item.fixed)
            {
                outString.append("length: fix, ");
                outString.append(QString("len_val: %1, ").arg(item.length));
            }
            else
            {
                outString.append("length: var, ");
            }

            if(item.match)
            {
                outString.append("match: yes: ");
                outString.append(item.matchBytes.toHex());
            }
            else
            {
                outString.append("match: no");
            }
        }
    }
}

void ParserWidget::setupUI()
{

    quint8 controlHeight = 28;

    // Assets
    expanded=true;

    nameEdit = new QLineEdit("Parser 1");
    nameEdit->setFixedHeight(controlHeight);

    statusBar = new QLabel("Ready");
    statusBar->setFixedHeight(controlHeight);

    addByteButton = new QPushButton("Add bytes");
    addByteButton->setFixedHeight(controlHeight);
    addNumberButton = new QPushButton("Add number");
    addNumberButton->setFixedHeight(controlHeight);
    addVectorButton = new QPushButton("Add vector");
    addVectorButton->setFixedHeight(controlHeight);

    expandButton = new QPushButton("Less");
    expandButton->setFixedWidth(50);
    expandButton->setFixedHeight(controlHeight);

    deleteButton = new QPushButton("Delete");
    deleteButton->setFixedWidth(80);
    deleteButton->setFixedHeight(controlHeight);

    vwList = new QList<VariableWidget*>;
    liveListWidget = new LiveListWidget(this);

    controlLayout = new QHBoxLayout;
    controlLayout->addWidget(nameEdit);
    controlLayout->addWidget(expandButton);
    controlLayout->addWidget(deleteButton);

    controlLayoutBottom = new QHBoxLayout;
    controlLayoutBottom->addWidget(statusBar);

    newItemLayout = new QHBoxLayout;
    newItemLayout->addWidget(addByteButton);
    newItemLayout->addWidget(addNumberButton);
    newItemLayout->addWidget(addVectorButton);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(controlLayoutBottom);
    mainLayout->addLayout(newItemLayout);
    mainLayout->addWidget(liveListWidget);
    setLayout(mainLayout);
    setMinimumWidth(500);
    setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    connect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
    connect(addByteButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(addNumberButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(addVectorButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(expandButton,SIGNAL(clicked()),this,SLOT(toggleExpand()));
    connect(deleteButton,SIGNAL(clicked()),this,SIGNAL(deleteParser()));
    connect(liveListWidget,SIGNAL(itemMoved(int,int,QListWidgetItem*)),this,SLOT(resorted(int,int,QListWidgetItem*)));
    connect(liveListWidget,SIGNAL(itemRemoved(int)),this,SLOT(itemRemoved(int)));
}

void ParserWidget::setupUI_fromParser()
{

    quint8 controlHeight = 28;

    // Assets
    expanded=true;

    nameEdit = new QLineEdit(parserUnit->getName());
    nameEdit->setFixedHeight(controlHeight);

    statusBar = new QLabel("Ready");
    statusBar->setFixedHeight(controlHeight);

    addByteButton = new QPushButton("Add bytes");
    addByteButton->setFixedHeight(controlHeight);
    addNumberButton = new QPushButton("Add number");
    addNumberButton->setFixedHeight(controlHeight);
    addVectorButton = new QPushButton("Add vector");
    addVectorButton->setFixedHeight(controlHeight);

    expandButton = new QPushButton("Less");
    expandButton->setFixedWidth(50);
    expandButton->setFixedHeight(controlHeight);

    deleteButton = new QPushButton("Delete");
    deleteButton->setFixedWidth(80);
    deleteButton->setFixedHeight(controlHeight);

    vwList = new QList<VariableWidget*>;
    liveListWidget = new LiveListWidget(this);

    controlLayout = new QHBoxLayout;
    controlLayout->addWidget(nameEdit);
    controlLayout->addWidget(expandButton);
    controlLayout->addWidget(deleteButton);

    controlLayoutBottom = new QHBoxLayout;
    controlLayoutBottom->addWidget(statusBar);

    newItemLayout = new QHBoxLayout;
    newItemLayout->addWidget(addByteButton);
    newItemLayout->addWidget(addNumberButton);
    newItemLayout->addWidget(addVectorButton);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(controlLayoutBottom);
    mainLayout->addLayout(newItemLayout);
    mainLayout->addWidget(liveListWidget);
    setLayout(mainLayout);
    setMinimumWidth(500);
    setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    connect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
    connect(addByteButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(addNumberButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(addVectorButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(expandButton,SIGNAL(clicked()),this,SLOT(toggleExpand()));
    connect(deleteButton,SIGNAL(clicked()),this,SIGNAL(deleteParser()));
    connect(liveListWidget,SIGNAL(itemMoved(int,int,QListWidgetItem*)),this,SLOT(resorted(int,int,QListWidgetItem*)));
    connect(liveListWidget,SIGNAL(itemRemoved(int)),this,SLOT(itemRemoved(int)));

}

