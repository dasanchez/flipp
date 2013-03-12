#include "parserlistwidget.h"

ParserListWidget::ParserListWidget(QWidget *parent) :
    QWidget(parent)
{
    parserList = new QList<ParserWidget*>;

    widgetNameLabel = new QLabel("Variable List");
    newParserButton = new QPushButton("New Parser");
    newParserButton->setFixedHeight(24);

    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);

    scrollArea = new QScrollArea;
    scrollAreaVLayout = new QVBoxLayout;
    saWidgetContents = new QWidget();

    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(saWidgetContents);
    scrollArea->setWidgetResizable(true);
    scrollAreaVLayout->addWidget(splitter);
    saWidgetContents->setLayout(scrollAreaVLayout);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newParserButton);

//    listWidget = new LiveListWidget(this);
//    listWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
//    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
//    mainLayout->addWidget(listWidget);
     mainLayout->addWidget(scrollArea);
     setLayout(mainLayout);

    connect(newParserButton,SIGNAL(clicked()),this,SLOT(newParser()));
//    connect(listWidget,SIGNAL(itemRemoved(int)),this,SLOT(itemRemoved(int)));
//    connect(listWidget,SIGNAL(itemMoved(int, int, QListWidgetItem*)),this,SLOT(resorted(int,int,QListWidgetItem*)));
}

void ParserListWidget::itemRemoved(int row)
{
    parserList->removeAt(row);

}

void ParserListWidget::sizeChanged(QSize newSize)
{
    ParserWidget *parser = static_cast<ParserWidget*>(QObject::sender());
    int row = parserList->indexOf(parser);

//    listWidget->item(row)->setSizeHint(newSize);
}

void ParserListWidget::resorted(int src, int dest, QListWidgetItem *item)
{
    parserList->insert(dest, parserList->takeAt(src));
}

void ParserListWidget::parserRemoved()
{
    ParserWidget* parser = static_cast<ParserWidget*>(QObject::sender());
     int row = parserList->indexOf(parser);
//     QListWidgetItem *item = listWidget->item(row);
     parserList->removeAt(row);
       parser->deleteLater();
//     listWidget->removeItemWidget(item);
//     listWidget->takeItem(row);
}

void ParserListWidget::newParser()
{
    ParserWidget *parser = new ParserWidget;
//    QListWidgetItem *item = new QListWidgetItem;
//    listWidget->addItem(item);
//    listWidget->setItemWidget(item,parser);
    splitter->addWidget(parser);

    parserList->append(parser);
//    connect(parser,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
    connect(parser,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
//    item->setSizeHint(parser->sizeHint());
}
