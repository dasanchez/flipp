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
#include "qterminaledit.h"
#include "../connection/connectionunit.h"

#define BOTH_VIEWS 0
#define ASCII_ONLY 1
#define HEX_ONLY 2

class ConnectionUnit;

class TerminalWidget : public QWidget
{
    Q_OBJECT
    
public:
    TerminalWidget(QWidget *parent = 0);
    ~TerminalWidget();

    void setViews(int);
    void setEcho(bool);
    void setPause(bool);
    void setPacketHexFormat(bool);
    QString currentConnection();

    bool paused;
    bool echoing;
    bool hexPacket;

public slots:
    void dataReceived(QByteArray);
    void updateConnections(QStringList);
    void assignConnection(ConnectionUnit *);

    void asciiTermToggled(bool);
    void hexTermToggled(bool);
    void toggleEcho();
    void togglePacketFormat();
    void changeConnection(QString);
    void detachConnection(void);
    void togglePause();

    void resizeTerminals(void);

signals:
    void sendData(QByteArray);
    void widgetRemoved(void);
    void terminalConnectionRequest(QString);
private:

    void setupUI();
    QTerminalEdit *asciiTerminal;
    QTerminalEdit *hexTerminal;
    QRegExp hexRegex;
    QString hex2char(QString);
    QString char2hex(QString);


    ConnectionUnit *connectionUnit;

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

    void updateEchoButton();
    void updatePauseButton();
    void updatePacketButton();
    void resizeEvent(QResizeEvent *);
private slots:
    void textEntered(QString, bool);

    void sendPacket(void);
    void remove(void);

};

#endif // TERMINALWIDGET_H
