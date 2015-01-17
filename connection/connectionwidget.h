#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QFrame>
#include <QPalette>
#include <QTimer>
#include <QStyledItemDelegate>

#include "connectionunit.h"

class ConnectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionWidget(QWidget *parent=0);
    ConnectionWidget(QWidget *parent, ConnectionUnit *cUnit = new ConnectionUnit);
    ~ConnectionWidget();
    ConnectionUnit *connectionUnit;
    QString getName(void);
    int getType(void);
    QString getIPAddress(void);
    quint32 getIPPort(void);
    QString getSerialPort(void);
    quint32 getBaudRate(void);
    void setName(QString);
    void setType(int);
    void setIPAddress(QString);
    void setIPPort(QString);
    void setSerialPort(QString);
    void setSerialBaud(QString);
    void setExpanded(bool);
    bool nameIsValid(void);

signals:
    void nameChange(void);
    void widgetRemoved(void);
    void dataRx(QByteArray);
    void sizeChange(QSize);
    
public slots:
    void setNameValid(bool);
    void nameEditChanged(QString);
    void dataTx(QByteArray);
    void toggleView(void);
    void signalData(void);
    void animateDataIcon(void);
    void remove();
    void togglePropertyFields(bool);
    void toggleType();

    // Data connection
    void typeChanged(QString newType);
    void addressChanged(QString);
    void portChanged(QString);
    void toggleConnection(void);
    void connectionChanged(connectionState);
    void errorReceived(QString);
    
private:
    void setupUI();
    void setupUI_fromConnection();

    // Control
    QPushButton *removeButton;
    QLineEdit *nameEdit;

    QString connectionName;
    QPushButton *connectButton;
    QPushButton *viewButton;

    // Connection

    QComboBox *serialPortCombo;
    QLabel *nameLabel;
    QLabel *addressLabel;
    QLabel *portLabel;
    QPushButton *typeButton;
    QLineEdit *addressEdit;
    QLineEdit *portEdit;
    QFrame *connFrame;
    bool validName;

    // Status
    QLabel *statusBar;
    QPushButton *dataIcon;
    quint16 dataIconShade;
    QTimer *iconTimer;

    // Layout
    QHBoxLayout *topLayout;
    QHBoxLayout *nameLayout;
    QHBoxLayout *addressLayout;
    QHBoxLayout *portLayout;
    QHBoxLayout *bottomLayout;
    QHBoxLayout *controlLayout;
    QVBoxLayout *mainLayout;

    bool isExpanded;

    void readType();
};

#endif // CONNECTIONWIDGET_H
