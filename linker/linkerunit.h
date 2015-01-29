#ifndef LINKERUNIT_H
#define LINKERUNIT_H

#include <QObject>
#include <QThread>
#include "../parser/parserengine.h"
#include "../connection/connectionunit.h"

class ParsedVariable{
public:
    int type;
    double value;
    QByteArray content;
};

class LinkerUnit : public QObject
{
    Q_OBJECT
public:
    LinkerUnit();
    QList<ParsedVariable> results;
    QString getConnectionName();
    QList<ComplexVariable> getVariables();
    QString getName();
    void setName(QString);
signals:
    void newDataPoint();
    void newData(VariableList parsedData);
public slots:
    void assignConnection(ConnectionUnit *);
    void detachConnection();
    void assignVariables(QList<ComplexVariable>);
    void parsedDataReady(VariableList parsedData);
private:
    ConnectionUnit *connectionUnit;
    ParserEngine *parserEngine;
    QString name;
    QThread *thread;

};

#endif // LINKERUNIT_H
