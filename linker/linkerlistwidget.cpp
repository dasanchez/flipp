#include "linkerlistwidget.h"

LinkerListWidget::LinkerListWidget(QWidget *parent) :
    QWidget(parent)
{

    linkerList = new QList<LinkerWidget*>;


    widgetNameLabel = new QLabel("Linkers");

    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);

    newLinkerButton = new QPushButton("New");
    newLinkerButton->setFixedHeight(24);
    newLinkerButton->setFixedWidth(100);

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
    topLayout->addWidget(newLinkerButton);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);

    setMinimumWidth(620);
    connect(newLinkerButton,SIGNAL(clicked()),this,SLOT(newLinker()));

}

void LinkerListWidget::newLinker()
{
    LinkerWidget *linker = new LinkerWidget;
//    parser->setName(newParserName());
    linkerList->append(linker);
    splitter->addWidget(linker);

//    connect(parser,SIGNAL(nameChange()),this,SLOT(nameChanged()));
//    connect(parser,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
//    connect(parser,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
//    updateList();
}

void LinkerListWidget::addLinker(LinkerWidget *liw)
{
    linkerList->append(liw);
    splitter->addWidget(liw);
//    connect(paw,SIGNAL(nameChange()),this,SLOT(nameChanged()));
//    connect(paw,SIGNAL(changeSize(QSize)),this,SLOT(sizeChanged(QSize)));
//    connect(paw,SIGNAL(deleteParser()),this,SLOT(parserRemoved()));
//    updateList();
}
