#include "plotterwidget.h"


PlotterWidget::PlotterWidget(QWidget *parent)
    : QWidget(parent)
{

    widgetNameLabel = new QLabel("Profiles");
    QFont font = widgetNameLabel->font();
    font.setPointSize(font.pointSize()+4);
    widgetNameLabel->setFont(font);

    newProfileButton = new QPushButton("New");
    newProfileButton->setFixedHeight(24);
    newProfileButton->setFixedWidth(100);

    connectionBox = new QComboBox;
    connectionBox->setItemDelegate(new QStyledItemDelegate);
    connectionBox->setFixedHeight(24);
    connectionBox->addItem("Connection 001");
    connectionBox->addItem("Connection 002");

    parserBox = new QComboBox;
    parserBox->setItemDelegate(new QStyledItemDelegate);
    parserBox->setFixedHeight(24);
    parserBox->addItem("Parser 001");

    tableWidget = new QTableWidget(this);
    tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    topLayout = new QHBoxLayout;
    topLayout->addWidget(widgetNameLabel);
    topLayout->addWidget(newProfileButton);

    dataSourceLayout = new QHBoxLayout;
    dataSourceLayout->addWidget(connectionBox);
    dataSourceLayout->addWidget(parserBox);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(dataSourceLayout);
    mainLayout->addWidget(tableWidget);

    this->setLayout(mainLayout);
//    setFixedWidth(410);

}

PlotterWidget::~PlotterWidget()
{

}
