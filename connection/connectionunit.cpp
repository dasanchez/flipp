#include "connectionunit.h"

ConnectionUnit::ConnectionUnit(QWidget *parent) :
    QWidget(parent)
{

#ifdef SERIAL_WINDOWS
    serialPrefix = QString("\\\\.\\");
#else
    serialPrefix.clear();
#endif
    tcpSocket = new QTcpSocket;
    udpSocket = new QUdpSocket;
    serialPort = new QSerialPort("COM3");
    connConfig.connType = TCP;
    connConfig.addr_port = QString("127.0.0.1");
    connConfig.port_baud = 50500;
    connConfig.connState = INACTIVE;
    connConfig.valid = true;
    connect(tcpSocket,SIGNAL(connected()),this,SLOT(tcpConnected()));
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataAvailable()));
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(dataAvailable()));

    // Handle errors
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorHandler(QAbstractSocket::SocketError)));
}

QString ConnectionUnit::getName()
{
    return connConfig.connName;
}

connectionType ConnectionUnit::getType()
{
    return connConfig.connType;
}

QString ConnectionUnit::getAddress_Port()
{
    return connConfig.addr_port;
}

quint32 ConnectionUnit::getPort_Baud()
{
    return connConfig.port_baud;
}

connectionState ConnectionUnit::getState()
{
    return connConfig.connState;
}

bool ConnectionUnit::isValid()
{
    return connConfig.valid;
}

void ConnectionUnit::setName(QString newName)
{
    connConfig.connName = newName;
}

void ConnectionUnit::setType(connectionType newType)
{
    connConfig.connType = newType;
}

void ConnectionUnit::setAddress_Port(QString newAddress)
{
    connConfig.addr_port = newAddress;
}

void ConnectionUnit::setPort_Baud(quint32 newPort)
{
    connConfig.port_baud = newPort;
}

void ConnectionUnit::setValid()
{
    connConfig.valid = true;
    emit validChange(true);
}

void ConnectionUnit::setInvalid()
{
    connConfig.valid = false;
    emit validChange(false);
}

void ConnectionUnit::tcpConnected(void)
{
    connConfig.connState = ACTIVE;
    connectionChanged();
}

void ConnectionUnit::dataConnect()
{
    QString serialName = serialPrefix;
    switch(connConfig.connType)
    {
    case TCP:
        tcpSocket->abort();
        tcpSocket->connectToHost(QHostAddress(connConfig.addr_port),connConfig.port_baud);
        connConfig.connState = CONNECTING;
        break;
    case UDP:
        udpSocket->bind(QHostAddress(connConfig.addr_port),connConfig.port_baud,QUdpSocket::ShareAddress);
        connConfig.connState = ACTIVE;
        break;
    case SERIAL:
        serialName.append(connConfig.addr_port);
        serialPort->setPortName(serialName);
        switch(connConfig.port_baud)
        {
        case 4800:
            serialPort->setBaudRate(QSerialPort::Baud4800);
            break;
        case 9600:
            serialPort->setBaudRate(QSerialPort::Baud9600);
            break;
        case 19200:
            serialPort->setBaudRate(QSerialPort::Baud19200);
            break;
        case 57600:
            serialPort->setBaudRate(QSerialPort::Baud57600);
            break;
        case 115200:
        default:
            serialPort->setBaudRate(QSerialPort::Baud115200);
            emit overrideBaud("115200");
            break;
        }
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setStopBits(QSerialPort::OneStop);
        if(serialPort->open(QIODevice::ReadWrite) == true)
        {
            connConfig.connState=ACTIVE;
            connect(serialPort,SIGNAL(readyRead()),this,SLOT(dataAvailable()));
            connect(serialPort,SIGNAL(aboutToClose()),this,SLOT(disconnected()));
        }
        else
        {
            connConfig.connState=INACTIVE;
            emit connectionError(QString("Serial Port could not be opened"));
        }
        break;
    default:
        break;
    }
    connectionChanged();
}

void ConnectionUnit::dataDisconnect()
{
    switch(connConfig.connType)
    {
    case TCP:
        tcpSocket->disconnectFromHost();
        tcpSocket->abort();
        connConfig.connState=INACTIVE;
        break;
    case UDP:
        udpSocket->close();
        udpSocket->abort();
        connConfig.connState=INACTIVE;
        break;
    case SERIAL:
        disconnect(serialPort,SIGNAL(readyRead()),this,SLOT(dataAvailable()));
        disconnect(serialPort,SIGNAL(aboutToClose()),this,SLOT(disconnected()));
        serialPort->close();
        connConfig.connState=INACTIVE;
        break;
    default:
        break;
    }
    connectionChanged();
}

void ConnectionUnit::dataAvailable()
{
    QByteArray bytebuf;
    QHostAddress sender;
    quint16 senderPort;
    switch(connConfig.connType)
    {
    case TCP:
        while(tcpSocket->bytesAvailable())
        {
            bytebuf.append(tcpSocket->readAll());
        }
        break;
    case UDP:
        while(udpSocket->bytesAvailable())
        {
            bytebuf.resize(udpSocket->pendingDatagramSize());
            udpSocket->readDatagram(bytebuf.data(), bytebuf.size(),
                                    &sender, &senderPort);
        }
    case SERIAL:
        while(serialPort->bytesAvailable())
        {
            bytebuf.append(serialPort->readAll());
        }
    default:
        break;
    }

    emit dataIn(bytebuf);
}

void ConnectionUnit::dataOut(QByteArray dataout)
{
    switch(connConfig.connType)
    {
    case TCP:
        if(tcpSocket->ConnectedState)
            tcpSocket->write(dataout);
        break;
    case UDP:
        if(udpSocket->ConnectedState)
            udpSocket->write(dataout);
        break;
    case SERIAL:
        if(serialPort->isOpen())
        {
            serialPort->write(dataout);
        }
        break;
    default:
        break;
    }
}

void ConnectionUnit::disconnected()
{
    connConfig.connState = INACTIVE;
    connectionChanged();
}

void ConnectionUnit::connectionChanged()
{
    emit connectionStatus(connConfig.connState);
}

void ConnectionUnit::errorHandler(QAbstractSocket::SocketError error)
{
    switch(error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        emit connectionError(QString("Connection was refused or timed out"));
        break;
    case QAbstractSocket::HostNotFoundError:
        emit connectionError(QString("Host address was not found"));
        break;
    case::QAbstractSocket::ClosingState:
        emit connectionError(QString("Closing State"));
        break;
    default:
        emit connectionError(QString("Connection error"));
        break;
    }
    connConfig.connState = INACTIVE;
    connectionChanged();
}
