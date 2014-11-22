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
#include "../parser/parserwidget.h"
#include "../parser/parserengine.h"
#include "../connection/connectionwidget.h"
#include <QThread>

class ParsedVariable{
public:
    QString name;
    int type;
    double value;
    QByteArray content;
};

//typedef QList<RepeatedVector> VariableList;

class LinkerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LinkerWidget(QWidget *parent = 0);
    QStringList connectionNamesList;
    QStringList parserNamesList;

    ~LinkerWidget();
    QString getConnection(void);
    QString getParser(void);
    QList<ComplexVariable> variables;

signals:
    void linkerConnectionRequest(QString);
    void linkerParserRequest(QString);
    void removeLinker();
    void newDataPoint(ParsedVariable);
public slots:

    void updateConnections(QStringList);
    void updateParsers(QStringList);
    void assignConnection(ConnectionWidget *);
    void changeConnection(QString);
    void detachConnection(void);
    void assignParser(ParserWidget *);
    void changeParser(QString);
    void detachParser(void);

    void newParserVariables(QList<ComplexVariable>);
    void populateParserTable();
    void parsedDataReady(VariableList parsedData);
    void testThread(int);
    void threadStarted();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *dataSourceLayout;

    QComboBox *connectionBox;
    QComboBox *parserBox;
    QPushButton *removeButton;
    QTableWidget *tableWidget;
    QList<QCheckBox*> *boxList;

    ParserEngine *parserEngine;
    QThread *thread;
    ConnectionWidget *connectionWidget;
    quint8 calcRowCount();
};

#endif // LINKERWIDGET_H
