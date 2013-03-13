#include "parserlistwidget.h"

ParserListWidget::ParserListWidget(QWidget *parent) :
    QWidget(parent)
{
    parserList = new QList<ParserWidget*>;

    widgetNameLabel = new QLabel("Parsers");

    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);

    newParserButton = new QPushButton("New");
    newParserButton->setFixedHeight(24);
    newParserButton->setFixedWidth(100);

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
}

void ParserListWidget::newParser()
{
    ParserWidget *parser = new ParserWidget;
    splitter->addWidget(parser);
    parserList->append(parser);
    connect(parser,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
    connect(parser,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
}
