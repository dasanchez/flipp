#include "parserlistwidget.h"

ParserListWidget::ParserListWidget(QWidget *parent, QList<ParserUnit*> *pUnits) :
    QWidget(parent),
    parsers(pUnits)
{
    parserList = new QList<ParserWidget*>;
    setupUI();
}

void ParserListWidget::itemRemoved(int row)
{
    parserList->removeAt(row);
}

void ParserListWidget::sizeChanged(QSize /*newSize*/)
{
    QList<int> sizes;
    foreach(ParserWidget *pw,*parserList)
    {
        sizes.append(pw->sizeHint().height());

    }
    splitter->setSizes(sizes);
    splitter->update();
}

void ParserListWidget::parserRemoved()
{
    ParserWidget* parser = static_cast<ParserWidget*>(QObject::sender());
    int row = parserList->indexOf(parser);
    parserList->removeAt(row);
    qDebug() << "Removing index " << parsers->indexOf(parser->parserUnit);
    parsers->removeAt(parsers->indexOf(parser->parserUnit));
    parser->deleteLater();
    checkAllNames();
    updateList();
}

void ParserListWidget::newParser()
{
    ParserUnit *pUnit = new ParserUnit;
    pUnit->setName(newParserName());
    parsers->append(pUnit);

    ParserWidget *parser = new ParserWidget(this,pUnit);

    parserList->append(parser);
    splitter->addWidget(parser);

    connect(parser,SIGNAL(nameChange(QString)),this,SLOT(nameChanged(QString)));
    connect(parser,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
    connect(parser,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
//    connect(pUnit,SIGNAL(variableListChanged()),this,SLOT(updateList()));
    updateList();
}

void ParserListWidget::addParser(ParserUnit *pUnit)
{
//    qDebug() << pUnit->variableList->size() << " variables in this parser";
    ParserWidget *parser = new ParserWidget(this,pUnit);
    parserList->append(parser);
    splitter->addWidget(parser);
    connect(parser,SIGNAL(nameChange(QString)),this,SLOT(nameChanged(QString)));
    connect(parser,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
    connect(parser,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
    updateList();
}

QString ParserListWidget::newParserName()
{
    QString newName;
    quint16 nameCounter=1;
    bool match;
    if(parserList->size()>0)
    {
        while(nameCounter<999)
        {
            match=false;
            newName=QString("Parser %1").arg(nameCounter);
            foreach(ParserUnit *parser, *parsers)
            {
                if(newName==parser->getName())
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
    return QString("Parser 1");
}

void ParserListWidget::nameChanged(QString newString)
{
    ParserWidget* parser = qobject_cast<ParserWidget*>(QObject::sender());

    quint16 index = parsers->indexOf(parser->parserUnit);
    parsers->at(index)->setNameValid(true);
    // Compare sender string against other parser names
    for(quint16 i=0;i<parsers->size();i++)
    {
        if(i!=index)
        {
            //            qDebug() << parsers->at(i)->getName() << " vs " << newString;
            if(parsers->at(i)->getName() == newString)
            {
                parsers->at(index)->setNameValid(false);
                //                qDebug() << "New name not OK";
            }
        }
    }

    checkAllNames();
    updateList();
}

void ParserListWidget::checkAllNames()
{
    bool duplicate;
    // Compare all connections to see if there are any we can validate
    for(quint16 i=0;i<parsers->size();i++)
    {
        duplicate = false;
        if(!parsers->at(i)->hasValidName())
        {
            //            qDebug() << "Attempting to clear parser # " << i;
            for(quint16 j=0;j<parsers->size();j++)
            {
                if(j!=i)
                {
                    if(parsers->at(i)->getName() == parsers->at(j)->getName())
                    {
                        duplicate=true;
                        j=parsers->size();
                    }
                }
            }
            if(!duplicate) parsers->at(i)->setNameValid(true);
        }
    }
}

void ParserListWidget::updateList()
{

//    qDebug() << "Parser list:";
//    foreach(ParserUnit *pUnit, *parsers)
//    {
//        qDebug() << pUnit->getName() << ": valid name: " << pUnit->hasValidName() << " variables:";
//        foreach(ComplexVariable *cVar, *pUnit->variableList)
//        {
//            qDebug() << "Name: " << cVar->name << "| Type: " << cVar->type
//                     << "| Fixed: " << cVar->fixed << "| Length: " << cVar->length;
//        }
//    }

    nameList.clear();
    foreach(ParserUnit *parser,*parsers)
    {
        if(parser->hasValidName())
            nameList.append(parser->getName());
    }

    emit parserListChanged(nameList);

}

void ParserListWidget::setupUI()
{
    widgetNameLabel = new QLabel("Parsers");

    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);

    newParserButton = new QPushButton("New");
    newParserButton->setFont(font);
    newParserButton->setFixedWidth(90);

    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    splitter->setHandleWidth(1);

    scrollArea = new QScrollArea;
    scrollAreaVLayout = new QVBoxLayout;
    scrollAreaVLayout->setSpacing(0);
    scrollAreaVLayout->setMargin(0);
    saWidgetContents = new QWidget();

    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(saWidgetContents);
    scrollArea->setWidgetResizable(true);
    scrollAreaVLayout->addWidget(splitter);
    saWidgetContents->setLayout(scrollAreaVLayout);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newParserButton);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);

    setMinimumWidth(680);
    connect(newParserButton,SIGNAL(clicked()),this,SLOT(newParser()));
}
