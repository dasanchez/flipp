#ifndef QDATACONNECTION_H
#define QDATACONNECTION_H

#include <QWidget>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QByteArray>
#include <QDebug>
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>

#ifdef SERIAL_WINDOWS
enum baudRate{B4800,B9600,B19200,B57600,B115200};
#endif
enum connectionType{TCP,UDP,SERIAL};
enum connectionState{INACTIVE,CONNECTING,ACTIVE};

struct connectionInfo
{
    connectionType connType;
    QString addr_port;
    quint32 port_baud;
    connectionState connState;
};

class QDataConnection : public QWidget
{
    Q_OBJECT
public:
    QDataConnection(QWidget *parent = 0);
    connectionType getType();
    QString getAddress_Port();
    quint32 getPort_Baud();
    connectionState getState();

    void setType(connectionType);
    void setAddress_Port(QString);
    void setPort_Baud(quint32);
    void setID(quint16);

signals:
    void dataIn(QByteArray);
    void serverDisconnect();
    void clientConnected();
    void connectionStatus(connectionState);
    void overrideBaud(QString);
    void connectionError(QString);

public slots:
    void dataConnect();
    void dataDisconnect();
    void dataOut(QByteArray);
    void tcpConnected();

private:
    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    QSerialPort *serialPort;
    connectionInfo connConfig;
    QString serialPrefix;

private slots:
    void dataAvailable();
    void disconnected();
    void connectionChanged();
    void errorHandler(QAbstractSocket::SocketError);

};

#endif // QDATACONNECTION_H
