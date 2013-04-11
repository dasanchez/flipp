#ifndef CONNECTIONLISTWIDGET_H
#define CONNECTIONLISTWIDGET_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QList>
#include <connection/connectionwidget.h>
#include <livelistwidget/livelistwidget.h>

class ConnectionListWidget : public QWidget
{
    Q_OBJECT

public:
    ConnectionListWidget(QWidget *parent = 0);
    ~ConnectionListWidget();
    QList<ConnectionWidget*> connectionList;
    QStringList *nameList;
    void sendData(quint8, QByteArray);
public slots:
        void newConnection(void);

        void nameChanged(void);
        void connectionRemoved(void);
        void itemRemoved(int row);
        void sizeChanged(QSize);
        void resorted(int src, int dest, QListWidgetItem* item);
signals:
    void connectionListChanged(QStringList*);
    void dataReceived(quint8, QByteArray);

private:
    QVBoxLayout *mainLayout;
    QPushButton *newConnBtn;
    QHBoxLayout *topLayout;
    QLabel *bottomLabel;
    QLabel *widgetNameLabel;

    LiveListWidget *listWidget;

    QString newConnectionName(void);
    void checkAllNames(void);
    void updateList(void);


};

#endif // CONNECTIONWIDGET_H
