#include "datastagingwidget.h"

DataStagingWidget::DataStagingWidget(QWidget *parent) :
    QWidget(parent)
{
    widgetNameLabel = new QLabel("Data Staging");
    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);
    newVariableButton = new QPushButton("New Variable");
    newVariableButton->setFixedHeight(24);
    newVariableButton->setFixedWidth(100);

    connectionBox = new QComboBox;
    parserBox = new QComboBox;

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newVariableButton);

    dataSourceLayout = new QHBoxLayout;
    dataSourceLayout->addWidget(connectionBox);
    dataSourceLayout->addWidget(parserBox);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(dataSourceLayout);
//    mainLayout->addWidget(scrollArea);
    this->setLayout(mainLayout);

}

DataStagingWidget::~DataStagingWidget()
{

}

void DataStagingWidget::updateConnections(QStringList connectionNames)
{
    connectionBox->clear();
    connectionBox->addItems(connectionNames);
//    int index = connectionBox->findText(connectionWidget->getName());
  //  if(index>=0)
  //      connectionBox->setCurrentIndex(index);
  //  changeConnection(connectionBox->currentText());

}
