#include "terminallistwidget.h"

TerminalListWidget::TerminalListWidget(QWidget *parent) :
    QWidget(parent)
{
    widgetNameLabel = new QLabel("Terminals");
    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);
    newTerminalButton = new QPushButton("New");
    newTerminalButton->setFixedHeight(24);
    newTerminalButton->setFixedWidth(100);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newTerminalButton);

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


    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    this->setLayout(mainLayout);

    setMinimumWidth(480);
    connect(newTerminalButton,SIGNAL(clicked()),this,SLOT(newTerminal()));
}

TerminalListWidget::~TerminalListWidget()
{

}

void TerminalListWidget::sizeChanged(QSize newSize)
{

}

void TerminalListWidget::newTerminal()
{
    TerminalWidget *termWidget = new TerminalWidget;

    termWidget->updateConnections(nameList);
    terminalList.append(termWidget);
    connect(termWidget,SIGNAL(widgetRemoved()),this,SLOT(terminalRemoved()));
    splitter->addWidget(termWidget);

    connect(termWidget,SIGNAL(terminalConnectionRequest(QString)),this,SLOT(terminalConnectionRequest(QString)));
}

void TerminalListWidget::terminalRemoved()
{
    TerminalWidget* term = qobject_cast<TerminalWidget *>(QObject::sender());
    disconnect(term,SIGNAL(terminalConnectionRequest(QString)),this,SLOT(terminalConnectionRequest(QString)));
    foreach(TerminalWidget *widget,terminalList)
    {
        if(widget==term)
        {
            terminalList.removeAt(terminalList.indexOf(widget));
        }
    }
}

void TerminalListWidget::updateConnections(QStringList connectionNames)
{
    nameList = connectionNames;
    foreach(TerminalWidget *terminal,terminalList)
    {
        terminal->updateConnections(nameList);
    }
}

void TerminalListWidget::terminalConnectionRequest(QString connectionName)
{
    TerminalWidget* terminal = qobject_cast<TerminalWidget *>(QObject::sender());
    emit terminalRequest(terminal,connectionName);
}
