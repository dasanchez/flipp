#include "parserwidget.h"
#include <QDebug>
#include <QDir>

ParserWidget::ParserWidget(QWidget *parent) :
    QWidget(parent)
{
    // Assets
    addByteIconPixmap = QPixmap(":/images/addbyte_icon.png");
    addNumberIconPixmap = QPixmap(":/images/addnumber_icon.png");
    addVectorIconPixmap = QPixmap(":/images/addvector_icon.png");
    moreIconPixmap = QPixmap(":/images/more_icon.png");
    lessIconPixmap = QPixmap(":/images/less_icon.png");
    delIconPixmap = QPixmap(":/images/delete_icon.png");

    expanded=true;

    nameEdit = new QLineEdit("Parser 1");
    nameEdit->setFixedHeight(24);
    statusBar = new QLabel("Ready");
//    statusBar->setFixedWidth(88);
    statusBar->setFixedHeight(24);
    //    addByteButton = new QPushButton;
    //    addByteButton->setIcon(QIcon(addByteIconPixmap));
    addByteButton = new QPushButton("Add bytes");
    addByteButton->setFixedHeight(24);
    addByteButton->setFixedWidth(88);
    //    addNumberButton = new QPushButton;
    //    addNumberButton->setIcon(QIcon(addNumberIconPixmap));
    addNumberButton = new QPushButton("Add number");
    addNumberButton->setFixedHeight(24);
    addNumberButton->setFixedWidth(88);
    //    addVectorButton = new QPushButton;
    //    addVectorButton->setIcon(QIcon(addVectorIconPixmap));
    addVectorButton = new QPushButton("Add vector");
    addVectorButton->setFixedHeight(24);
    addVectorButton->setFixedWidth(88);
    //    expandButton = new QPushButton;
    //    expandButton->setIcon(QIcon(moreIconPixmap));
    expandButton = new QPushButton("Less");
    expandButton->setFixedSize(40,24);
    //    deleteButton = new QPushButton;
    //    deleteButton->setIcon(QIcon(delIconPixmap));
    deleteButton = new QPushButton("Delete");
    deleteButton->setFixedSize(60,24);


    vwList = new QList<VariableWidget*>;
    lw = new LiveListWidget(this);


    controlLayout = new QHBoxLayout;
    controlLayout->addWidget(nameEdit);
//    controlLayout->addWidget(statusBar);
    controlLayout->addWidget(addByteButton);
    controlLayout->addWidget(addNumberButton);
    controlLayout->addWidget(addVectorButton);
    controlLayout->addWidget(expandButton);
    controlLayout->addWidget(deleteButton);

    controlLayoutBottom = new QHBoxLayout;
    controlLayoutBottom->addWidget(statusBar);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(controlLayoutBottom);
    mainLayout->addWidget(lw);
    setLayout(mainLayout);
    setMinimumWidth(500);
    setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    connect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
    connect(addByteButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(addNumberButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(addVectorButton,SIGNAL(clicked()),this,SLOT(newVariable()));
    connect(expandButton,SIGNAL(clicked()),this,SLOT(toggleExpand()));
    connect(deleteButton,SIGNAL(clicked()),this,SIGNAL(deleteParser()));
    connect(lw,SIGNAL(itemMoved(int,int,QListWidgetItem*)),this,SLOT(resorted(int,int,QListWidgetItem*)));
    connect(lw,SIGNAL(itemRemoved(int)),this,SLOT(itemRemoved(int)));

}

ParserWidget::~ParserWidget()
{
    //    delete ui;
}

QString ParserWidget::getName()
{
    return nameEdit->text();
}

bool ParserWidget::hasValidName()
{
    return validName;
}

void ParserWidget::setNameValid(bool isValid)
{
    validName = isValid;
    if(validName)
    {
        statusBar->setText("Ready");
    }
    else
    {
        statusBar->setText("Name is not valid");
    }
}

void ParserWidget::setName(QString newName)
{
    disconnect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
    nameEdit->setText(newName);
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
}

void ParserWidget::toggleExpand()
{
    expanded=!expanded;
    if(expanded)
    {
        mainLayout->addWidget(lw);
        lw->setVisible(true);
        //        expandButton->setIcon(QIcon(moreIconPixmap));
        expandButton->setText("Less");
    }
    else
    {
        mainLayout->removeWidget(lw);
        lw->setVisible(false);
        //        expandButton->setIcon(QIcon(lessIconPixmap));
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

    printList();
    emit updateVariableList(variableList);
}


void ParserWidget::newVariable()
{
    QPushButton *senderButton = static_cast<QPushButton*>(QObject::sender());
    VariableWidget *vw = new VariableWidget(lw);
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
    QListWidgetItem *item = new QListWidgetItem(lw);

    lw->addItem(item);
    item->setSizeHint(vw->sizeHint());
    lw->setItemWidget(item,vw);

    connect(vw,SIGNAL(sizeToggled(QSize)),this,SLOT(itemSize(QSize)));
    connect(vw,SIGNAL(deleteVar()),this,SLOT(remVariable()));
    connect(vw,SIGNAL(variableChanged()),this,SLOT(variableListChanged()));
    variableListChanged();
}

void ParserWidget::addVariableWidget(VariableWidget *vw)
{
    vwList->append(vw);
    variableList.append(vw->variable);
    QListWidgetItem *item = new QListWidgetItem(lw);
    lw->addItem(item);
    item->setSizeHint(vw->sizeHint());
    lw->setItemWidget(item,vw);

    connect(vw,SIGNAL(sizeToggled(QSize)),this,SLOT(itemSize(QSize)));
    connect(vw,SIGNAL(deleteVar()),this,SLOT(remVariable()));
    connect(vw,SIGNAL(variableChanged()),this,SLOT(variableListChanged()));
    variableListChanged();
}

void ParserWidget::remVariable()
{
    VariableWidget *vw = static_cast<VariableWidget*>(QObject::sender());
    int row = vwList->indexOf(vw);
    QListWidgetItem *item = lw->item(row);
    variableList.removeAt(row);
    lw->removeItemWidget(item);
    lw->takeItem(row);
    vwList->removeAt(row);
    delete vw;
    variableListChanged();
}

void ParserWidget::itemSize(QSize newSize)
{
    VariableWidget *vw = static_cast<VariableWidget*>(QObject::sender());
    int row = vwList->indexOf(vw);
    QListWidgetItem *item = lw->item(row);
    item->setSizeHint(newSize);
}

void ParserWidget::resorted(int src,int dest,QListWidgetItem* item)
{
    VariableWidget *vw = static_cast<VariableWidget*>(lw->itemWidget(item));

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
    //    qDebug() << "variables:";
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
        //        qDebug() << outString;
    }
}

