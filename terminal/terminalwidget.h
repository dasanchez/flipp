#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QStyledItemDelegate>
#include <terminal/qterminaledit.h>
#include <connection/connectionwidget.h>
//#include "connectionwidget.h"

class ConnectionWidget;

class TerminalWidget : public QWidget
{
    Q_OBJECT
    
public:
    TerminalWidget(QWidget *parent = 0);
    ~TerminalWidget();

public slots:
    void dataReceived(QByteArray);
    void updateConnections(QStringList);
    void assignConnection(ConnectionWidget *);
signals:
    void sendData(QByteArray);
    void widgetRemoved(void);
    void terminalConnectionRequest(QString);
private:
    void resizeEvent(QResizeEvent *);
    void setupUI();
    QTerminalEdit *asciiTerminal;
    QTerminalEdit *hexTerminal;
    QRegExp hexRegex;
    QString hex2char(QString);
    QString char2hex(QString);
    bool paused;
    bool echoing;
    bool hexPacket;

    ConnectionWidget *connectionWidget;

    // UI controls:
    // Control
    QPushButton *removeButton;
    QComboBox *connectionBox;
    QPushButton *echoButton;
    QPushButton *asciiButton;
    QPushButton *hexButton;
    QPushButton *pauseButton;
    QPushButton *clearButton;
    // Packet
    QPushButton *hexPacketButton;
    QLineEdit *packetEdit;
    QPushButton *sendButton;

    // Layout:
    QHBoxLayout *controlLayout;
    QHBoxLayout *aschexLayout;
    QHBoxLayout *packetLayout;
    QHBoxLayout *terminalLayout;
    QVBoxLayout *mainLayout;
    QFrame *frame;

    // Assets:
    QPixmap delIconPixmap;
    QPixmap echoOnIconPixmap;
    QPixmap echoOffIconPixmap;
    QPixmap playIconPixmap;
    QPixmap pauseIconPixmap;
    QPixmap clearIconPixmap;
    QPixmap ascIconPixmap;
    QPixmap hexIconPixmap;
    QPixmap sendIconPixmap;


private slots:
    void textEntered(QString, bool);
    void toggleEcho();
    void togglePacketFormat();
    void resizeTerminals(void);
    void asciiTermToggled(bool);
    void hexTermToggled(bool);
    void sendPacket(void);
    void remove(void);
    void changeConnection(QString);
    void detachConnection(void);
    void togglePause();
};

#endif // TERMINALWIDGET_H
