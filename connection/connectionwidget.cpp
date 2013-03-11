#include "connectionwidget.h"

ConnectionWidget::ConnectionWidget(QWidget *parent) :
    QWidget(parent)
{
    tcpIconPixmap = QPixmap("../QConnect/tcp_icon.png");
    udpIconPixmap = QPixmap("../QConnect/udp_icon.png");
    comIconPixmap = QPixmap("../QConnect/com_icon.png");
    moreIconPixmap = QPixmap("../QConnect/more_icon.png");
    lessIconPixmap = QPixmap("../QConnect/less_icon.png");
    delIconPixmap = QPixmap("../QConnect/delete_icon.png");
    connOffIconPixmap = QPixmap("../QConnect/connoff_icon.png");
    connOnIconPixmap = QPixmap("../QConnect/connon_icon.png");
    inOutIconPixmap = QPixmap("../QConnect/inout_icon.png");

    isExpanded=true;

    setupUI();
    toggleType();
    toggleType();
    addressEdit->setText("COM3");
    addressChanged("COM3");

    // Data connection Signals
    connect(typeButton,SIGNAL(clicked()),this,SLOT(toggleType()));
    connect(addressEdit,SIGNAL(textChanged(QString)),this,SLOT(addressChanged(QString)));
    connect(portEdit,SIGNAL(textChanged(QString)),this,SLOT(portChanged(QString)));
    connect(connectButton,SIGNAL(clicked()),this,SLOT(toggleConnection()));
    connect(dataConnection,SIGNAL(connectionStatus(connectionState)),this,SLOT(connectionChanged(connectionState)));
    connect(dataConnection,SIGNAL(dataIn(QByteArray)),this,SLOT(signalData()));
    connect(dataConnection,SIGNAL(dataIn(QByteArray)),this,SIGNAL(dataRx(QByteArray)));
    connect(dataConnection,SIGNAL(overrideBaud(QString)),portEdit,SLOT(setText(QString)));
    connect(dataConnection,SIGNAL(connectionError(QString)),this,SLOT(errorReceived(QString)));

    // Styling
//    QFile qss("../QConnect/connectionwidget.css");
//    qss.open(QFile::ReadOnly);
//    setStyleSheet(qss.readAll());
//    qss.close();
}

ConnectionWidget::~ConnectionWidget()
{
    dataConnection->dataDisconnect();
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
        viewButton->setIcon(QIcon(moreIconPixmap));
    }
    else
    {
        mainLayout->removeItem(controlLayout);
        viewButton->setIcon(QIcon(lessIconPixmap));
    }
    mainLayout->addLayout(bottomLayout);

    emit sizeChange(this->sizeHint());
}

void ConnectionWidget::dataTx(QByteArray dataOut)
{
    if(dataConnection->getState()==ACTIVE)
    {
        dataConnection->dataOut(dataOut);
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

void ConnectionWidget::setName(QString newName)
{
    disconnect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
    nameEdit->setText(newName);
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
}

void ConnectionWidget::toggleType()
{
    switch(dataConnection->getType())
    {

    case UDP:
        dataConnection->setType(SERIAL);
        typeButton->setIcon(QIcon(comIconPixmap));
        addressLabel->setText("Port");
        portLabel->setText("Baud");
        break;
    case SERIAL:
        dataConnection->setType(TCP);
        typeButton->setIcon(QIcon(tcpIconPixmap));
        addressLabel->setText("Address");
        portLabel->setText("Port");
        break;
    default:
        dataConnection->setType(UDP);
        typeButton->setIcon(QIcon(udpIconPixmap));
        addressLabel->setText("Address");
        portLabel->setText("Port");
        break;
    }
}

void ConnectionWidget::typeChanged(QString newType)
{
    if(newType==QString("TCP"))
    {
        dataConnection->setType(TCP);
        addressLabel->setText("IP address");
        portLabel->setText("Port");
    }
    if(newType==QString("UDP"))
    {
        dataConnection->setType(UDP);
        addressLabel->setText("IP address");
        portLabel->setText("Port");
    }
    if(newType==QString("Serial"))
    {
        dataConnection->setType(SERIAL);
        addressLabel->setText("Serial port");
        portLabel->setText("Baud rate");
    }
}

void ConnectionWidget::addressChanged(QString newAddress)
{
    dataConnection->setAddress_Port(newAddress);
}

void ConnectionWidget::portChanged(QString newPort)
{
    QString r = "\\D";
    newPort = newPort.remove(QRegExp(r));
    portEdit->setText(newPort);
    dataConnection->setPort_Baud(newPort.toLong());
}

void ConnectionWidget::toggleConnection()
{
    if(dataConnection->getState() == INACTIVE)
    {
        dataConnection->dataConnect();
    }
    else if(dataConnection->getState() == ACTIVE)
    {
        dataConnection->dataDisconnect();
        statusBar->setText("Click the Connect button");
    }
}

void ConnectionWidget::connectionChanged(connectionState newState)
{
    if(newState == ACTIVE)
    {
        connectButton->setIcon(QIcon(connOnIconPixmap));
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
        connectButton->setIcon(QIcon(connOffIconPixmap));
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

void ConnectionWidget::setupUI()
{
    // Control
    nameEdit = new QLineEdit("Connection 1");
    nameEdit->setToolTip("Enter a connection name");
    nameEdit->setMinimumWidth(200);
    nameEdit->setFixedHeight(24);

    connectButton = new QPushButton;
    connectButton->setToolTip("Open connection");
    connectButton->setIcon(QIcon(connOffIconPixmap));
    connectButton->setFixedWidth(24);
    connectButton->setFixedHeight(24);
    viewButton = new QPushButton;
    viewButton->setIcon(QIcon(moreIconPixmap));
    viewButton->setFixedWidth(24);
    viewButton->setFixedHeight(24);
    viewButton->setToolTip("Hide or show connection properties");
    removeButton = new QPushButton;
    removeButton->setIcon(QIcon(delIconPixmap));
    removeButton->setFixedWidth(24);
    removeButton->setFixedHeight(24);
    removeButton->setToolTip("Remove connection");

    // Connection
    dataConnection = new QDataConnection;
    addressLabel = new QLabel("Address");
    addressLabel->setFixedWidth(50);
    addressLabel->setFixedHeight(24);
    addressLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    addressLabel->setAlignment(Qt::AlignCenter);
    portLabel = new QLabel("Port");
    portLabel->setFixedWidth(40);
    portLabel->setFixedHeight(24);
    portLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    portLabel->setAlignment(Qt::AlignCenter);
    typeButton = new QPushButton;
    typeButton->setToolTip("Toggle between TCP, UDP, and COM connection");
    typeButton->setFixedHeight(24);
    typeButton->setFixedHeight(24);
    typeButton->setIcon(QIcon(tcpIconPixmap));
    typeButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    addressEdit = new QLineEdit("127.0.0.1");
    addressEdit->setToolTip("Enter an IP adress or serial port");
    addressEdit->setMinimumWidth(80);
    addressEdit->setFixedHeight(24);
    addressEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    portEdit = new QLineEdit("52292");
    portEdit->setToolTip("Enter an IP port or baud rate");
    portEdit->setMinimumWidth(60);
    portEdit->setFixedHeight(24);
    portEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    // Status
    statusBar = new QLabel("Click the Connect button");
    statusBar->setFixedHeight(24);
    statusBar->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    dataIcon = new QPushButton;
    dataIcon->setIcon(QIcon(inOutIconPixmap));
    dataIcon->setFixedHeight(24);
    dataIcon->setFixedWidth(24);
    dataIconShade = 255;
    dataIcon->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    iconTimer = new QTimer;
    iconTimer->setInterval(50);
    validName = true;

    // Layout
    topLayout = new QHBoxLayout;
    topLayout->addWidget(nameEdit);
    topLayout->addWidget(connectButton);
    topLayout->addWidget(viewButton);
    topLayout->addWidget(removeButton);
    topLayout->setMargin(0);

    controlLayout = new QHBoxLayout;
    controlLayout->addWidget(typeButton);
    controlLayout->addWidget(addressLabel);
    controlLayout->addWidget(addressEdit);
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
    this->setFixedWidth(360);
    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    // UI SIGNALS
    connect(viewButton,SIGNAL(clicked()),this,SLOT(toggleView()));
    connect(iconTimer,SIGNAL(timeout()),this,SLOT(animateDataIcon()));
    connect(removeButton,SIGNAL(clicked()),this,SLOT(remove()));
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(nameChange()));
}
