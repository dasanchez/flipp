#include "datastagingwidget.h"

DataStagingWidget::DataStagingWidget(QWidget *parent) :
    QWidget(parent)
{
    connectionLabel = new QLabel("Connection");
    connectionBox = new QComboBox;
    connectionBox->addItem("TCP GPS");

    parserLabel = new QLabel("Parser");
    parserBox = new QComboBox;
    parserBox->addItem("GPS Parser");

    linkLayout = new QHBoxLayout;

    linkLayout->addWidget(connectionLabel);
    linkLayout->addWidget(connectionBox);
    linkLayout->addWidget(parserLabel);
    linkLayout->addWidget(parserBox);

    setLayout(linkLayout);

}
