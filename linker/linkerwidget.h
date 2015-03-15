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
#include <QThread>
#include "../parser/parserwidget.h"
#include "../parser/parserengine.h"
#include "../connection/connectionunit.h"
#include "../linker/linkerunit.h"

class LinkerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LinkerWidget(QWidget *parent = 0);
    LinkerWidget(QWidget *parent, LinkerUnit *linkerUnit);
    QStringList connectionNamesList;
    QStringList parserNamesList;

    LinkerUnit *linkerUnit;

    ~LinkerWidget();
    QString getConnection(void);
    QString getParser(void);
    QList<ComplexVariable*> *variables;
    QList<ParsedVariable> results;


signals:
    void linkerConnectionRequest(QString);
    void linkerParserRequest(QString);
    void removeLinker();
    void newDataPoint();
public slots:

    void updateConnections(QStringList);
    void updateParsers(QStringList);
    void setConnection(QString);
    void assignConnection(ConnectionUnit *);
    void changeConnection(QString);
    void detachConnection(void);
//    void assignParser(ParserUnit *);
    void changeParser(QString);
    void detachParser(void);

//    void newParserVariables(QList<ComplexVariable>);
    void populateParserTable();
    void linkerDataReady();

//    void parsedDataReady(VariableList parsedData);
//    void threadStarted();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *dataSourceLayout;

    QComboBox *connectionBox;
    QComboBox *parserBox;
    QPushButton *removeButton;
    QTableWidget *tableWidget;
//    QList<QCheckBox*> *boxList;

//    ParserEngine *parserEngine;
//    QThread *thread;
//    ConnectionWidget *connectionWidget;
//    ConnectionUnit *connectionUnit;
    quint8 calcRowCount();

    void setupUI();
};

#endif // LINKERWIDGET_H
