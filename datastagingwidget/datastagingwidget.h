#ifndef DATASTAGINGWIDGET_H
#define DATASTAGINGWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QComboBox>
#include <QTableWidget>
#include <parser/parserwidget.h>
#include <parser/parserengine.h>
#include <connection/connectionwidget.h>
#include <QThread>

//typedef QList<RepeatedVector> VariableList;

class DataStagingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataStagingWidget(QWidget *parent = 0);
    QStringList connectionNamesList;
    QStringList parserNamesList;

    ~DataStagingWidget();
signals:
    void stagingConnectionRequest(QString);
    void stagingParserRequest(QString);
public slots:

    void updateConnections(QStringList);
    void updateParsers(QStringList);
    void assignConnection(ConnectionWidget *);
    void changeConnection(QString);
    void detachConnection(void);
    void assignParser(ParserWidget *);
    void changeParser(QString);
    void detachParser(void);

    void populateParserTable();
    void parsedDataReady(VariableList parsedData);
    void testThread(int);
    void threadStarted();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    QPushButton *newVariableButton;
    QLabel *bottomLabel;
    QLabel *widgetNameLabel;

    QComboBox *connectionBox;
    QComboBox *parserBox;

    QTableWidget *tableWidget;

    QHBoxLayout *dataSourceLayout;

    //    QScrollArea *scrollArea;
    //    QVBoxLayout *scrollAreaVLayout;
    //    QWidget *saWidgetContents;
    ParserWidget *parserWidget;
    ParserEngine *parserEngine;
    QThread *thread;
    ConnectionWidget *connectionWidget;
    quint8 calcRowCount();
};

#endif // DATASTAGINGWIDGET_H
