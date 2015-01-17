#include "terminalwidget.h"

TerminalWidget::TerminalWidget(QWidget *parent) :
    QWidget(parent)
{
    connectionUnit = new ConnectionUnit;
    paused = false;
    echoing=false;
    hexPacket=false;

    setupUI();

    asciiTerminal->setMaximumWidth(this->size().width()/4);
    connect(removeButton,SIGNAL(clicked()),this,SLOT(remove()));
    connect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));
    connect(asciiButton,SIGNAL(toggled(bool)),this,SLOT(asciiTermToggled(bool)));
    connect(hexButton,SIGNAL(toggled(bool)),this,SLOT(hexTermToggled(bool)));
    connect(asciiButton,SIGNAL(toggled(bool)),this,SLOT(resizeTerminals()));
    connect(hexButton,SIGNAL(toggled(bool)),this,SLOT(resizeTerminals()));
    connect(echoButton,SIGNAL(clicked()),this,SLOT(toggleEcho()));
    connect(pauseButton,SIGNAL(clicked()),this,SLOT(togglePause()));
    connect(clearButton,SIGNAL(clicked()),asciiTerminal,SLOT(clear()));
    connect(clearButton,SIGNAL(clicked()),hexTerminal,SLOT(clear()));
    connect(hexPacketButton,SIGNAL(clicked()),this,SLOT(togglePacketFormat()));
    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendPacket()));
    connect(asciiTerminal,SIGNAL(textEntered(QString,bool)),this,SLOT(textEntered(QString,bool)));
    connect(hexTerminal,SIGNAL(textEntered(QString,bool)),this,SLOT(textEntered(QString,bool)));
}

TerminalWidget::~TerminalWidget()
{

}

void TerminalWidget::updateEchoButton()
{
    if(echoing)
    {
        echoButton->setChecked(true);
    }
    else
    {
        echoButton->setChecked(false);
    }
}

void TerminalWidget::setEcho(bool echoOn)
{
    echoing=echoOn;
    asciiTerminal->setEcho(echoing);
    hexTerminal->setEcho(echoing);
    updateEchoButton();
}

void TerminalWidget::toggleEcho()
{
    echoing = !echoing;
    asciiTerminal->setEcho(echoing);
    hexTerminal->setEcho(echoing);
    updateEchoButton();
}

void TerminalWidget::updatePauseButton()
{
    if(paused)
    {
        pauseButton->setChecked(true);
    }
    else
    {
        pauseButton->setChecked(false);
    }
}

void TerminalWidget::setPause(bool pauseOn)
{
    paused=pauseOn;
    updatePauseButton();
}

QString TerminalWidget::currentConnection()
{
    return connectionBox->currentText();
}

void TerminalWidget::togglePause()
{
    paused=!paused;
    updatePauseButton();
}

void TerminalWidget::updatePacketButton()
{
    if(hexPacket)
    {
        packetEdit->setText(char2hex(packetEdit->text()));
        hexPacketButton->setText("ASCII");
    }
    else
    {

        packetEdit->setText(hex2char(packetEdit->text()));
        hexPacketButton->setText("HEX");
    }
}

void TerminalWidget::setPacketHexFormat(bool hexFormat)
{
    hexPacket = hexFormat;
    updatePacketButton();
}

void TerminalWidget::togglePacketFormat()
{
    hexPacket = !hexPacket;
    updatePacketButton();
}

void TerminalWidget::changeConnection(QString connection)
{
    disconnect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));
    connectionBox->setCurrentText(connection);
    connect(connectionBox,SIGNAL(activated(QString)),this,SLOT(changeConnection(QString)));

    disconnect(connectionUnit,SIGNAL(dataIn(QByteArray)),this,SLOT(dataReceived(QByteArray)));
    disconnect(this,SIGNAL(sendData(QByteArray)),connectionUnit,SLOT(dataOut(QByteArray)));
    emit terminalConnectionRequest(connection);
}

void TerminalWidget::remove()
{
    emit widgetRemoved();

    this->deleteLater();
}

void TerminalWidget::resizeEvent(QResizeEvent *ev)
{
    resizeTerminals();
    ev->accept();
}

void TerminalWidget::resizeTerminals()
{
    QSize newSize = this->size();
    if(asciiTerminal->isVisible())
    {
        if(hexTerminal->isVisible())
        {
            asciiTerminal->setMinimumWidth(newSize.width()/4);
            asciiTerminal->setMaximumWidth(newSize.width()/4);
        }
        else
        {
            asciiTerminal->setMaximumWidth(newSize.width());
        }
    }
    else
    {
        if(hexTerminal->isVisible())
        {
            hexTerminal->setMaximumWidth(newSize.width());
        }
        else
        {

        }
    }
    this->update();
}

void TerminalWidget::updateConnections(QStringList connectionNames)
{
    connectionBox->clear();
    connectionBox->addItems(connectionNames);
    int index = connectionBox->findText(connectionUnit->getName());
    if(index>=0)
        connectionBox->setCurrentIndex(index);
    changeConnection(connectionBox->currentText());
}

void TerminalWidget::textEntered(QString newText, bool isHex)
{
    QByteArray data;
    data.append(newText);

    if(isHex)
    {
        if(hexTerminal->willEcho())
            asciiTerminal->appendText(data,true);
    }
    else
    {
        if(asciiTerminal->willEcho())
            hexTerminal->appendText(data,true);
    }
    QByteArray output;
    output.append(data);
    emit sendData(output);

}

void TerminalWidget::setViews(int views)
{
    disconnect(asciiButton,SIGNAL(toggled(bool)),this,SLOT(asciiTermToggled(bool)));
    disconnect(hexButton,SIGNAL(toggled(bool)),this,SLOT(hexTermToggled(bool)));
    disconnect(asciiButton,SIGNAL(toggled(bool)),this,SLOT(resizeTerminals()));
    disconnect(hexButton,SIGNAL(toggled(bool)),this,SLOT(resizeTerminals()));

    switch(views)
    {
    case BOTH_VIEWS:
        asciiButton->setChecked(true);
        hexButton->setChecked(true);
        asciiButton->setDisabled(false);
        hexButton->setDisabled(false);
        asciiTerminal->setVisible(true);
        hexTerminal->setVisible(true);
        break;
    case ASCII_ONLY:
        asciiButton->setChecked(true);
        hexButton->setChecked(false);
        asciiButton->setDisabled(true);
        asciiTerminal->setVisible(true);
        hexTerminal->setVisible(false);
        break;
    case HEX_ONLY:
        asciiButton->setChecked(false);
        hexButton->setChecked(true);
        hexButton->setDisabled(true);
        asciiTerminal->setVisible(false);
        hexTerminal->setVisible(true);
        break;
    default:
        break;
    }
    connect(asciiButton,SIGNAL(toggled(bool)),this,SLOT(asciiTermToggled(bool)));
    connect(hexButton,SIGNAL(toggled(bool)),this,SLOT(hexTermToggled(bool)));
    connect(asciiButton,SIGNAL(toggled(bool)),this,SLOT(resizeTerminals()));
    connect(hexButton,SIGNAL(toggled(bool)),this,SLOT(resizeTerminals()));
    resizeTerminals();
}

void TerminalWidget::asciiTermToggled(bool on)
{
    if(on)
    {
        hexButton->setDisabled(false);
        asciiTerminal->setVisible(true);
    }
    else
    {
        hexButton->setDisabled(true);
        asciiTerminal->setVisible(false);
    }
}

void TerminalWidget::hexTermToggled(bool on)
{
    if(on)
    {
        asciiButton->setDisabled(false);
        hexTerminal->setVisible(true);
    }
    else
    {
        asciiButton->setDisabled(true);
        hexTerminal->setVisible(false);
    }
}

void TerminalWidget::dataReceived(QByteArray dataIn)
{
    if(!paused)
    {
        asciiTerminal->appendText(dataIn);
        hexTerminal->appendText(dataIn);
    }
}

void TerminalWidget::sendPacket()
{
    QString displayPacket = packetEdit->text();
    QByteArray output;
    if(!displayPacket.isEmpty())
    {
        if(hexPacket)
        {
            output.append(displayPacket);
            output = QByteArray::fromHex(output);
        }
        else
        {
            output.append(displayPacket);
        }
        if(echoing)
        {
            asciiTerminal->appendText(output,true);
            hexTerminal->appendText(output,true);
        }
        emit sendData(output);
    }
}

QString TerminalWidget::char2hex(QString characters)
{
    QString hexChars;
    // Switch all characters to hex (00-ff) representation
    while(characters.size()>0)
    {
        QByteArray temparray;
        temparray.append(characters.left(1));
        hexChars.append(temparray.toHex().toUpper());
        if(characters.size()>1)
        {
            hexChars.append(" ");
        }
        characters.remove(0,1);
    }
    return hexChars;
}

QString TerminalWidget::hex2char(QString hexChars)
{
    // Change all hex data back to ascii representation
    QByteArray characters;
    hexRegex.indexIn(hexChars);
    QString temp = hexRegex.cap();

    // Remove all spaces from string
    temp=temp.simplified();
    temp = temp.replace(' ',"");

    // Use a QByteArray to convert
    characters.append(temp);
    characters = QByteArray::fromHex(characters);

    return characters;
}

void TerminalWidget::assignConnection(ConnectionUnit *newConnection)
{
    connectionUnit=newConnection;
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),this,SLOT(dataReceived(QByteArray)));
    connect(this,SIGNAL(sendData(QByteArray)),connectionUnit,SLOT(dataOut(QByteArray)));
    connect(connectionUnit,SIGNAL(destroyed()),this,SLOT(detachConnection()));
}

void TerminalWidget::detachConnection()
{
    connectionUnit = new ConnectionUnit;
}

void TerminalWidget::setupUI()
{
    quint8 controlHeight = 28;

    asciiTerminal = new QTerminalEdit;
//    asciiTerminal->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    hexTerminal = new QTerminalEdit;
    hexTerminal->setHex(true);
    hexRegex.setPattern(QString("([A-Fa-f0-9]{2}( )?)+"));

    // UI controls

    connectionBox = new QComboBox;
    connectionBox->setItemDelegate(new QStyledItemDelegate);
    connectionBox->setToolTip("Select a connection");
    connectionBox->setMinimumWidth(100);
    connectionBox->setFixedHeight(controlHeight);

    asciiButton = new QPushButton("ASCII");
    asciiButton->setToolTip("Show or hide the ASCII terminal");
    asciiButton->setCheckable(true);
    asciiButton->setChecked(true);
    asciiButton->setObjectName("asciiButton");
    asciiButton->setFixedWidth(72);
    asciiButton->setFixedHeight(controlHeight);

    hexButton = new QPushButton("HEX");
    hexButton->setToolTip("Show or hide the hex terminal");
    hexButton->setCheckable(true);
    hexButton->setChecked(false);
    hexButton->setObjectName("hexButton");
    hexButton->setFixedWidth(72);
    hexButton->setFixedHeight(controlHeight);

    echoButton = new QPushButton("Echo");
    echoButton->setToolTip("Toggle echo");
    echoButton->setFixedWidth(60);
    echoButton->setFixedHeight(controlHeight);
    echoButton->setObjectName("echoButton");
    echoButton->setCheckable(true);

    pauseButton = new QPushButton("Pause");
    pauseButton->setToolTip("Pause terminal display");
    pauseButton->setFixedWidth(60);
    pauseButton->setFixedHeight(controlHeight);
    pauseButton->setCheckable(true);
    pauseButton->setObjectName("pauseButton");

    clearButton = new QPushButton("Clear");
    clearButton->setToolTip("Clear terminal views");
    clearButton->setFixedWidth(60);
    clearButton->setFixedHeight(controlHeight);

    removeButton = new QPushButton("Delete");
    removeButton->setToolTip("Remove terminal");
    removeButton->setFixedWidth(60);
    removeButton->setFixedHeight(controlHeight);

    packetEdit = new QLineEdit;
    packetEdit->setToolTip("Enter packet to transmit");
    packetEdit->setFixedHeight(controlHeight);

    hexPacketButton = new QPushButton("HEX");
    hexPacketButton->setFixedWidth(60);
    hexPacketButton->setFixedHeight(controlHeight);
    hexPacketButton->setToolTip("Toggle hex display");

    sendButton = new QPushButton("Send");
    sendButton->setToolTip("Transmit packet");
    sendButton->setFixedWidth(50);
    sendButton->setFixedHeight(controlHeight);

    // Layout:
    controlLayout = new QHBoxLayout;
    controlLayout->addWidget(connectionBox);

    aschexLayout = new QHBoxLayout;
    aschexLayout->addWidget(asciiButton);
    aschexLayout->addWidget(hexButton);
    aschexLayout->setSpacing(0);

    controlLayout->addLayout(aschexLayout);
    controlLayout->addWidget(echoButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(clearButton);
    controlLayout->addWidget(removeButton);

    packetLayout = new QHBoxLayout;
    packetLayout->addWidget(packetEdit);
    packetLayout->addWidget(hexPacketButton);
    packetLayout->addWidget(sendButton);

    terminalLayout = new QHBoxLayout;
    terminalLayout->addWidget(asciiTerminal);
    terminalLayout->addWidget(hexTerminal);
//     asciiTerminal->set(this->size().width());
    hexTerminal->setVisible(false);

    frame = new QFrame;
    frame->setFrameStyle(QFrame::HLine | QFrame::Raised);

    mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(packetLayout);
    mainLayout->addLayout(terminalLayout);
//    mainLayout->addWidget(frame);
    this->setLayout(mainLayout);
//    resizeTerminals();

}
