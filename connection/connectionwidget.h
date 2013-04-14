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

#include <connection/qdataconnection.h>

class ConnectionWidget : public QWidget
{
    Q_OBJECT
public:
    ConnectionWidget(QWidget *parent = 0);
    ~ConnectionWidget();
    QString getName(void);
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

    // Control
    QPushButton *removeButton;
    QLineEdit *nameEdit;

    QString connectionName;
    QPushButton *connectButton;
    QPushButton *viewButton;

    // Connection
    QDataConnection *dataConnection;
    QComboBox *serialPortCombo;
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
    QHBoxLayout *bottomLayout;
    QHBoxLayout *controlLayout;
    QVBoxLayout *mainLayout;

    bool isExpanded;

    // Assets
    QPixmap tcpIconPixmap;
    QPixmap udpIconPixmap;
    QPixmap comIconPixmap;
    QPixmap moreIconPixmap;
    QPixmap lessIconPixmap;
    QPixmap delIconPixmap;
    QPixmap connOffIconPixmap;
    QPixmap connOnIconPixmap;
    QPixmap inOutIconPixmap;


};

#endif // CONNECTIONWIDGET_H
