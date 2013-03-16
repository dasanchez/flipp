#ifndef PLOTTERWIDGET_H
#define PLOTTERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QList>
#include <QTableWidget>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <connection/connectionwidget.h>
#include <parser/parserwidget.h>
#include <livelistwidget/livelistwidget.h>

class PlotterWidget : public QWidget
{
    Q_OBJECT

public:
    PlotterWidget(QWidget *parent = 0);
    ~PlotterWidget();

public slots:
    void updateConnections(QStringList);
    void updateParsers(QStringList*);
    void assignConnection(ConnectionWidget *);
    void assignParser(ParserWidget *);
signals:
    void plotterConnectionRequest(QString);
    void plotterParserRequest(QString);
private:

    QLabel *widgetNameLabel;
    QPushButton *newProfileButton;
    QComboBox *connectionBox;
    QComboBox *parserBox;
    QTableWidget *tableWidget;

    QHBoxLayout *topLayout;
    QHBoxLayout *dataSourceLayout;
    QVBoxLayout *mainLayout;

    ConnectionWidget *connectionWidget;
    ParserWidget *parserWidget;

private slots:
     void changeConnection(QString);
     void detachConnection(void);
     void changeParser(QString);
     void populateParserTable();
};

#endif // CONNECTIONWIDGET_H
