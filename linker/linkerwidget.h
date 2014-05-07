#ifndef LINKERWIDGET_H
#define LINKERWIDGET_H

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

class LinkerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LinkerWidget(QWidget *parent = 0);
    QStringList connectionNamesList;
    QStringList parserNamesList;

    ~LinkerWidget();
signals:
    void linkerConnectionRequest(QString);
    void linkerParserRequest(QString);
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
//    QHBoxLayout *topLayout;
//    QPushButton *newVariableButton;
    QLabel *bottomLabel;
//    QLabel *widgetNameLabel;

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

#endif // LINKERWIDGET_H
