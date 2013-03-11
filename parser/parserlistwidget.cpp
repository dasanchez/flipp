#include "parserlistwidget.h"

ParserListWidget::ParserListWidget(QWidget *parent) :
    QWidget(parent)
{
    widgetNameLabel = new QLabel("Variable List");
    newParserButton = new QPushButton("New Parser");
    newParserButton->setFixedHeight(24);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newParserButton);

    listWidget = new LiveListWidget(this);
    listWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(listWidget);
}
