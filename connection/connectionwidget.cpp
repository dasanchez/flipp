#include "connectionwidget.h"

ConnectionWidget::ConnectionWidget(QWidget *parent) :
    QWidget(parent)
{
    isExpanded=true;

    // Connection
    connectionUnit = new ConnectionUnit;

    setupUI();

    // Data connection Signals
    connect(typeButton,SIGNAL(clicked()),this,SLOT(toggleType()));
    connect(addressEdit,SIGNAL(textChanged(QString)),this,SLOT(addressChanged(QString)));
    connect(serialPortCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(addressChanged(QString)));
    connect(portEdit,SIGNAL(textChanged(QString)),this,SLOT(portChanged(QString)));
    connect(connectButton,SIGNAL(clicked()),this,SLOT(toggleConnection()));
    connect(connectionUnit,SIGNAL(connectionStatus(connectionState)),this,SLOT(connectionChanged(connectionState)));
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),this,SLOT(signalData()));
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),this,SIGNAL(dataRx(QByteArray)));
    connect(connectionUnit,SIGNAL(overrideBaud(QString)),portEdit,SLOT(setText(QString)));
    connect(connectionUnit,SIGNAL(connectionError(QString)),this,SLOT(errorReceived(QString)));
    connect(connectionUnit,SIGNAL(validChange(bool)),this,SLOT(setNameValid(bool)));

    emit sizeChange(this->sizeHint());
}

ConnectionWidget::ConnectionWidget(QWidget *parent, ConnectionUnit *cUnit) :
    QWidget(parent),
    connectionUnit(cUnit)
{
    isExpanded=true;

    setupUI_fromConnection();

    // Data connection Signals
    connect(connectionUnit,SIGNAL(connectionStatus(connectionState)),this,SLOT(connectionChanged(connectionState)));
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),this,SLOT(signalData()));
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),this,SIGNAL(dataRx(QByteArray)));
    connect(connectionUnit,SIGNAL(overrideBaud(QString)),portEdit,SLOT(setText(QString)));
    connect(connectionUnit,SIGNAL(connectionError(QString)),this,SLOT(errorReceived(QString)));
    connect(connectionUnit,SIGNAL(validChange(bool)),this,SLOT(setNameValid(bool)));

    emit sizeChange(this->sizeHint());


}

ConnectionWidget::~ConnectionWidget()
{
    connectionUnit->dataDisconnect();
}

void ConnectionWidget::remove()
{
    emit widgetRemoved();
    this->deleteLater();
}

QString ConnectionWidget::getName()
{
    return nameEdit->text();
}

int ConnectionWidget::getType()
{
    return connectionUnit->getType();
}

QString ConnectionWidget::getIPAddress()
{
    return connectionUnit->getAddress_Port();
}

quint32 ConnectionWidget::getIPPort()
{
    return connectionUnit->getPort_Baud();
}

QString ConnectionWidget::getSerialPort()
{
    return connectionUnit->getAddress_Port();
}

quint32 ConnectionWidget::getBaudRate()
{
    return connectionUnit->getPort_Baud();
}

void ConnectionWidget::nameEditChanged(QString newName)
{
    disconnect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameEditChanged(QString)));
    connectionUnit->setName(newName);
    emit nameChange();
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameEditChanged(QString)));
}

bool ConnectionWidget::nameIsValid(void)
{
    return validName;
}

void ConnectionWidget::toggleView()
{
    isExpanded=!isExpanded;
    typeButton->setVisible(isExpanded);
    addressLabel->setVisible(isExpanded);
    addressEdit->setVisible(isExpanded);
    portLabel->setVisible(isExpanded);
    portEdit->setVisible(isExpanded);

    mainLayout->removeItem(bottomLayout);
    if(isExpanded)
    {
        mainLayout->addLayout(controlLayout);
        viewButton->setText("Less");
    }
    else
    {
        mainLayout->removeItem(controlLayout);
        viewButton->setText("More");
    }
    mainLayout->addLayout(bottomLayout);

    emit sizeChange(this->sizeHint());
}

void ConnectionWidget::dataTx(QByteArray dataOut)
{
    if(connectionUnit->getState()==ACTIVE)
    {
        connectionUnit->dataOut(dataOut);
        signalData();
    }
}

void ConnectionWidget::signalData()
{
    // Start IO animation
    dataIconShade=200;
    QString styleString = QString("QPushButton{background-color: rgb(%1,%1,%1)}").arg(dataIconShade);
    dataIcon->setStyleSheet(styleString);
    iconTimer->start();
}

void ConnectionWidget::animateDataIcon()
{
    dataIconShade-=15;
    QString styleString = QString("QPushButton{background-color: rgb(%1,%1,%1)}").arg(dataIconShade);
    dataIcon->setStyleSheet(styleString);
    if(dataIconShade<=30)
    {
        iconTimer->stop();
        dataIconShade=0;
        styleString = QString("QPushButton{background-color: rgb(%1,%1,%1)}").arg(dataIconShade);
        dataIcon->setStyleSheet(styleString);
    }
}

void ConnectionWidget::setNameValid(bool valid)
{
    if(valid)
    {
        validName = true;
        connectButton->setEnabled(true);
        statusBar->setText("Click the Connect button");
    }
    else
    {
        validName = false;
        connectButton->setEnabled(false);
        statusBar->setText("Name is not valid");
    }
}

void ConnectionWidget::setType(int newType)
{
    switch(newType)
    {
    case TCP:
        connectionUnit->setType(TCP);
        typeButton->setText("TCP");
        if(isExpanded)
        {
            serialPortCombo->setVisible(false);
            addressEdit->setVisible(true);
        }
        addressLabel->setText("Address");
        portLabel->setText("Port");
        addressChanged(addressEdit->text());
        break;
    case UDP:
        connectionUnit->setType(UDP);
        typeButton->setText("UDP");
        if(isExpanded)
        {
            serialPortCombo->setVisible(false);
            addressEdit->setVisible(true);
        }
        addressLabel->setText("Address");
        portLabel->setText("Port");
        addressChanged(addressEdit->text());
        break;
    case SERIAL:
        connectionUnit->setType(SERIAL);
        typeButton->setText("SERIAL");
        if(isExpanded)
        {
            addressEdit->setVisible(false);
            serialPortCombo->setVisible(true);
        }
        addressLabel->setText("Port");
        portLabel->setText("Baud");
        addressChanged(serialPortCombo->currentText());
        break;
    default:
        break;
    }

}

void ConnectionWidget::toggleType()
{
    switch(connectionUnit->getType())
    {

    case UDP:
        connectionUnit->setType(SERIAL);
        typeButton->setText("SERIAL");
        if(isExpanded)
        {
            addressEdit->setVisible(false);
            serialPortCombo->setVisible(true);
        }
        addressLabel->setText("Port");
        portLabel->setText("Baud");
        addressChanged(serialPortCombo->currentText());
        break;
    case SERIAL:
        connectionUnit->setType(TCP);
        typeButton->setText("TCP");
        if(isExpanded)
        {
            serialPortCombo->setVisible(false);
            addressEdit->setVisible(true);
        }
        addressLabel->setText("Address");
        portLabel->setText("Port");
        addressChanged(addressEdit->text());
        break;
    default:
        connectionUnit->setType(UDP);
        typeButton->setText("UDP");
        break;
    }

}

void ConnectionWidget::typeChanged(QString newType)
{
    if(newType==QString("TCP"))
    {
        connectionUnit->setType(TCP);
        addressLabel->setText("Address");
        portLabel->setText("Port");
    }
    if(newType==QString("UDP"))
    {
        connectionUnit->setType(UDP);
        addressLabel->setText("Address:");
        portLabel->setText("Port");
    }
    if(newType==QString("SERIAL"))
    {
        connectionUnit->setType(SERIAL);
        addressLabel->setText("Port");
        portLabel->setText("Baud");
    }

}

void ConnectionWidget::readType()
{
    switch(connectionUnit->getType())
    {
    case TCP:
        typeButton->setText("TCP");
        break;
    case UDP:
        typeButton->setText("UDP");
        break;
    case SERIAL:
        typeButton->setText("SERIAL");
        break;
    default:
        break;
    }

}

void ConnectionWidget::setIPAddress(QString address)
{
    disconnect(addressEdit,SIGNAL(textChanged(QString)),this,SLOT(addressChanged(QString)));
    addressEdit->setText(address);
    connect(addressEdit,SIGNAL(textChanged(QString)),this,SLOT(addressChanged(QString)));
    connectionUnit->setAddress_Port(address);
}

void ConnectionWidget::setSerialPort(QString port)
{
    disconnect(serialPortCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(addressChanged(QString)));
    // Check if combo box has the port requested
    int index = serialPortCombo->findText(port,Qt::MatchFixedString);
    if(index>=0)
    {
        serialPortCombo->setCurrentIndex(index);
        connectionUnit->setAddress_Port(serialPortCombo->currentText());
    }
    else
    {
        serialPortCombo->setCurrentIndex(0);
        connectionUnit->setAddress_Port(serialPortCombo->currentText());
    }
    connect(serialPortCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(addressChanged(QString)));
}

void ConnectionWidget::addressChanged(QString newAddress)
{
    connectionUnit->setAddress_Port(newAddress);
}

void ConnectionWidget::setIPPort(QString ipPort)
{
    disconnect(portEdit,SIGNAL(textChanged(QString)),this,SLOT(portChanged(QString)));
    portChanged(ipPort);
    connect(portEdit,SIGNAL(textChanged(QString)),this,SLOT(portChanged(QString)));
}

void ConnectionWidget::setSerialBaud(QString serBaud)
{
    disconnect(portEdit,SIGNAL(textChanged(QString)),this,SLOT(portChanged(QString)));
    portChanged(serBaud);
    connect(portEdit,SIGNAL(textChanged(QString)),this,SLOT(portChanged(QString)));
}

void ConnectionWidget::portChanged(QString newPort)
{
    portEdit->setText(newPort);
    connectionUnit->setPort_Baud(newPort.toLong());
}

void ConnectionWidget::toggleConnection()
{
    if(connectionUnit->getState() == INACTIVE)
    {
        connectionUnit->dataConnect();
    }
    else if(connectionUnit->getState() == ACTIVE)
    {
        connectionUnit->dataDisconnect();
        statusBar->setText("Click the Connect button");
    }
}

void ConnectionWidget::connectionChanged(connectionState newState)
{
    if(newState == ACTIVE)
    {
        connectButton->setText("Disconnect");
        connectButton->setEnabled(true);
        connectButton->setToolTip("Close connection");
        statusBar->setText("Connected");
        togglePropertyFields(false);
    }
    else if(newState == CONNECTING)
    {
        connectButton->setEnabled(false);
        statusBar->setText("Establishing connection");
        togglePropertyFields(false);
    }
    else
    {
        connectButton->setText("Connect");
        connectButton->setEnabled(true);
        connectButton->setToolTip("Open connection");
        togglePropertyFields(true);
    }
}

void ConnectionWidget::errorReceived(QString error)
{
    statusBar->setText(error);
}

void ConnectionWidget::togglePropertyFields(bool enabled)
{
    nameEdit->setEnabled(enabled);
    typeButton->setEnabled(enabled);
    addressEdit->setEnabled(enabled);
    portEdit->setEnabled(enabled);
}

void ConnectionWidget::setExpanded(bool expand)
{
    isExpanded=expand;
    typeButton->setVisible(isExpanded);
    addressLabel->setVisible(isExpanded);
    addressEdit->setVisible(isExpanded);
    portLabel->setVisible(isExpanded);
    portEdit->setVisible(isExpanded);

    mainLayout->removeItem(bottomLayout);
    if(isExpanded)
    {
        mainLayout->addLayout(controlLayout);
    }
    else
    {
        mainLayout->removeItem(controlLayout);
    }
    mainLayout->addLayout(bottomLayout);

    emit sizeChange(this->sizeHint());

}

void ConnectionWidget::setupUI_fromConnection()
{
    quint8 controlHeight = 26;
    // Control
    connectButton = new QPushButton("Connect");
    connectButton->setToolTip("Open connection");
    connectButton->setFixedWidth(96);
    connectButton->setFixedHeight(controlHeight);

    nameLabel = new QLabel("Name");
    nameLabel->setObjectName("connNameLabel");
    nameLabel->setFixedWidth(78);
    nameLabel->setFixedHeight(controlHeight);
    nameLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    nameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);
//    nameLabel->setAlignment(Qt::AlignRight);

    nameEdit = new QLineEdit(connectionUnit->getName());
    nameEdit->setObjectName("connNameEdit");
    nameEdit->setToolTip("Enter a connection name");
    nameEdit->setMinimumWidth(160);
    nameEdit->setFixedHeight(controlHeight);
    nameEdit->setAlignment(Qt::AlignCenter);

    viewButton = new QPushButton("Less");
    viewButton->setFixedWidth(50);
    viewButton->setFixedHeight(controlHeight);
    viewButton->setToolTip("Hide or show connection properties");

    removeButton = new QPushButton("Delete");
    removeButton->setFixedWidth(80);
    removeButton->setFixedHeight(controlHeight);
    removeButton->setToolTip("Remove connection");

    typeButton = new QPushButton;
    typeButton->setFixedWidth(96);
    typeButton->setFixedHeight(controlHeight);
    readType();
    typeButton->setToolTip("Toggle between TCP, UDP, and COM connection");

    addressLabel = new QLabel("Address");
    addressLabel->setObjectName("connAddrLabel");
    addressLabel->setFixedWidth(78);
    addressLabel->setFixedHeight(controlHeight);
    addressLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    addressLabel->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

    addressEdit = new QLineEdit(connectionUnit->getAddress_Port());
    addressEdit->setObjectName("connAddrEdit");
    addressEdit->setToolTip("Enter an IP adress or serial port");
    addressEdit->setMinimumWidth(160);
    addressEdit->setFixedHeight(controlHeight);
    addressEdit->setAlignment(Qt::AlignHCenter);

    serialPortCombo = new QComboBox;
    serialPortCombo->setObjectName("connAddrCombo");
    serialPortCombo->setItemDelegate(new QStyledItemDelegate);
    serialPortCombo->setMinimumWidth(160);
    serialPortCombo->setFixedHeight(controlHeight);
    // Populate with available serial ports
    QList<QSerialPortInfo> spinfo = QSerialPortInfo::availablePorts();
    QStringList portNames;
    foreach(QSerialPortInfo info,spinfo)
    {
        portNames.append(info.portName());
    }
    serialPortCombo->addItems(portNames);
    serialPortCombo->setVisible(false);
    serialPortCombo->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    portLabel = new QLabel("Port");
    portLabel->setObjectName("connPortLabel");
    portLabel->setFixedWidth(50);
    portLabel->setFixedHeight(controlHeight);
    portLabel->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

    portEdit = new QLineEdit(QString("%1").arg(connectionUnit->getPort_Baud()));
    portEdit->setObjectName("connPortEdit");
    portEdit->setToolTip("Enter an IP port or baud rate");
    portEdit->setFixedWidth(80);
    portEdit->setFixedHeight(controlHeight);
    portEdit->setAlignment(Qt::AlignHCenter);

    // Status
    statusBar = new QLabel("Click the Connect button");
    statusBar->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    statusBar->setFixedHeight(controlHeight);

    dataIcon = new QPushButton("IO");
    dataIcon->setFixedWidth(32);
    dataIcon->setFixedHeight(controlHeight);
    dataIconShade = 255;
    iconTimer = new QTimer;
    iconTimer->setInterval(50);
    validName = true;

    // Layout
    topLayout = new QHBoxLayout;
    topLayout->addWidget(connectButton);
//    topLayout->addWidget(nameLabel);
//    topLayout->addWidget(nameEdit);
    nameLayout = new QHBoxLayout;
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    nameLayout->setSpacing(0);
    topLayout->addLayout(nameLayout);
    topLayout->addWidget(viewButton);
    topLayout->addWidget(removeButton);
    topLayout->setMargin(2);

    controlLayout = new QHBoxLayout;
    controlLayout->addWidget(typeButton);
    addressLayout = new QHBoxLayout;
    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(addressEdit);
    addressLayout->addWidget(serialPortCombo);
    addressLayout->setSpacing(0);
    controlLayout->addLayout(addressLayout);
    portLayout = new QHBoxLayout;
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portEdit);
    portLayout->setSpacing(0);
    controlLayout->addLayout(portLayout);

    bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(statusBar);
    bottomLayout->addWidget(dataIcon);
    bottomLayout->setMargin(0);

    mainLayout = new QVBoxLayout;

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(bottomLayout);

    this->setLayout(mainLayout);
    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);
    this->setFixedWidth(500);


    // UI SIGNALS
    connect(viewButton,SIGNAL(clicked()),this,SLOT(toggleView()));
    connect(iconTimer,SIGNAL(timeout()),this,SLOT(animateDataIcon()));
    connect(removeButton,SIGNAL(clicked()),this,SLOT(remove()));
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameEditChanged(QString)));
    connect(typeButton,SIGNAL(clicked()),this,SLOT(toggleType()));
    connect(addressEdit,SIGNAL(textChanged(QString)),this,SLOT(addressChanged(QString)));
    connect(serialPortCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(addressChanged(QString)));
    connect(portEdit,SIGNAL(textChanged(QString)),this,SLOT(portChanged(QString)));
    connect(connectButton,SIGNAL(clicked()),this,SLOT(toggleConnection()));
}



void ConnectionWidget::setupUI()
{
    // Control
    connectButton = new QPushButton("Connect");
    connectButton->setToolTip("Open connection");
    connectButton->setFixedWidth(96);
    connectButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
    connectButton->setFixedHeight(100);

    nameLabel = new QLabel("Name:");
    nameLabel->setFixedWidth(78);
    nameLabel->setFixedHeight(36);
//    nameLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    nameLabel->setAlignment(Qt::AlignRight);

    nameEdit = new QLineEdit("Connection 1");
    nameEdit->setToolTip("Enter a connection name");
    nameEdit->setMinimumWidth(92);
    nameEdit->setFixedHeight(40);
    nameEdit->setAlignment(Qt::AlignCenter);

    viewButton = new QPushButton("Less");
    viewButton->setFixedWidth(50);
    viewButton->setToolTip("Hide or show connection properties");

    removeButton = new QPushButton("Delete");
    removeButton->setFixedWidth(80);
    removeButton->setToolTip("Remove connection");

    typeButton = new QPushButton;
    typeButton->setFixedWidth(96);
    readType();
    typeButton->setToolTip("Toggle between TCP, UDP, and COM connection");

    addressLabel = new QLabel("Address:");
    addressLabel->setFixedWidth(78);
    addressLabel->setAlignment(Qt::AlignRight);

    portLabel = new QLabel("Port:");
    portLabel->setFixedWidth(50);
    portLabel->setAlignment(Qt::AlignRight);

    addressEdit = new QLineEdit("127.0.0.1");
    addressEdit->setToolTip("Enter an IP adress or serial port");
    addressEdit->setMinimumWidth(160);
    addressEdit->setAlignment(Qt::AlignHCenter);

    serialPortCombo = new QComboBox;
    serialPortCombo->setItemDelegate(new QStyledItemDelegate);
    serialPortCombo->setMinimumWidth(160);
    // Populate with available serial ports
    QList<QSerialPortInfo> spinfo = QSerialPortInfo::availablePorts();
    QStringList portNames;
    foreach(QSerialPortInfo info,spinfo)
    {
        portNames.append(info.portName());
    }
    serialPortCombo->addItems(portNames);
    serialPortCombo->setVisible(false);
    serialPortCombo->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    portEdit = new QLineEdit("50500");
    portEdit->setToolTip("Enter an IP port or baud rate");
    portEdit->setFixedWidth(80);
    portEdit->setAlignment(Qt::AlignHCenter);

    // Status
    statusBar = new QLabel("Click the Connect button");
//    statusBar->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    dataIcon = new QPushButton("IO");
    dataIcon->setFixedWidth(32);
    dataIconShade = 255;
    iconTimer = new QTimer;
    iconTimer->setInterval(50);
    validName = true;

    // Layout
    topLayout = new QHBoxLayout;
    topLayout->addWidget(connectButton);
    topLayout->addWidget(nameLabel);
    topLayout->addWidget(nameEdit);
    topLayout->addWidget(viewButton);
    topLayout->addWidget(removeButton);
    topLayout->setMargin(2);

    controlLayout = new QHBoxLayout;
    controlLayout->addWidget(typeButton);
    controlLayout->addWidget(addressLabel);
    controlLayout->addWidget(addressEdit);
    controlLayout->addWidget(serialPortCombo);
    controlLayout->addWidget(portLabel);
    controlLayout->addWidget(portEdit);

    bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(statusBar);
    bottomLayout->addWidget(dataIcon);
    bottomLayout->setMargin(0);

    mainLayout = new QVBoxLayout;

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(bottomLayout);

    this->setLayout(mainLayout);
    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);
    this->setFixedWidth(500);


    // UI SIGNALS
    connect(viewButton,SIGNAL(clicked()),this,SLOT(toggleView()));
    connect(iconTimer,SIGNAL(timeout()),this,SLOT(animateDataIcon()));
    connect(removeButton,SIGNAL(clicked()),this,SLOT(remove()));
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameEditChanged(QString)));
    connect(typeButton,SIGNAL(clicked()),this,SLOT(toggleType()));
    connect(addressEdit,SIGNAL(textChanged(QString)),this,SLOT(addressChanged(QString)));
    connect(serialPortCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(addressChanged(QString)));
    connect(portEdit,SIGNAL(textChanged(QString)),this,SLOT(portChanged(QString)));
    connect(connectButton,SIGNAL(clicked()),this,SLOT(toggleConnection()));
}
