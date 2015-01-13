#include "parserlistwidget.h"

ParserListWidget::ParserListWidget(QWidget *parent) :
    QWidget(parent)
{
    parserList = new QList<ParserWidget*>;
//    nameList = new QStringList;

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

    setMinimumWidth(620);
    connect(newParserButton,SIGNAL(clicked()),this,SLOT(newParser()));
}

void ParserListWidget::itemRemoved(int row)
{
    parserList->removeAt(row);

}

void ParserListWidget::sizeChanged(QSize newSize)
{
    QList<int> sizes;
    foreach(ParserWidget *pw,*parserList)
    {
        sizes.append(pw->sizeHint().height());

    }
    splitter->setSizes(sizes);
    splitter->update();
}

void ParserListWidget::resorted(int src, int dest, QListWidgetItem *item)
{
    parserList->insert(dest, parserList->takeAt(src));
}

void ParserListWidget::parserRemoved()
{
    ParserWidget* parser = static_cast<ParserWidget*>(QObject::sender());
    int row = parserList->indexOf(parser);
    parserList->removeAt(row);
    parser->deleteLater();
    checkAllNames();
    updateList();
}

void ParserListWidget::newParser()
{
    ParserWidget *parser = new ParserWidget;
    parser->setName(newParserName());
    parserList->append(parser);
    splitter->addWidget(parser);

    connect(parser,SIGNAL(nameChange()),this,SLOT(nameChanged()));
    connect(parser,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
    connect(parser,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
    updateList();
}

void ParserListWidget::addParser(ParserWidget *paw)
{
    parserList->append(paw);
    splitter->addWidget(paw);
    connect(paw,SIGNAL(nameChange()),this,SLOT(nameChanged()));
    connect(paw,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
    connect(paw,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
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
            foreach(ParserWidget *parser, *parserList)
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

void ParserListWidget::nameChanged()
{
    ParserWidget* parser = qobject_cast<ParserWidget*>(QObject::sender());
    // Compare sender string against other connections
    quint16 index = parserList->indexOf(parser);
    for(quint16 i=0;i<parserList->size();i++)
    {
        if(i!=index)
        {
            if(parser->getName() == parserList->at(i)->getName())
            {
                parser->setNameValid(false);
                return;
            }
        }
    }
    parser->setNameValid(true);

    checkAllNames();
    updateList();
}

void ParserListWidget::checkAllNames()
{
    bool valid;
    // Compare all connections to see if there are any we can validate
    for(quint16 i=0;i<parserList->size();i++)
    {
        valid=true;
        if(!parserList->at(i)->hasValidName())
        {
            for(quint16 j=0;j<parserList->size();j++)
            {
                if(j==i) break;
                if(parserList->at(i)->getName() == parserList->at(j)->getName())
                {
                    valid=false;
                }
            }
            parserList->at(i)->setNameValid(valid);
        }
    }
}

void ParserListWidget::updateList()
{
    nameList.clear();
    foreach(ParserWidget *parser,*parserList)
    {
        nameList.append(parser->getName());
    }
    emit parserListChanged(nameList);
}
