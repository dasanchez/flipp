#include "linkerunit.h"

LinkerUnit::LinkerUnit()
{
    connectionUnit = new ConnectionUnit;
    parserEngine = new ParserEngine;
    thread = new QThread;
    parserEngine->moveToThread(thread);

    connect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
    thread->start();
}

void LinkerUnit::assignConnection(ConnectionUnit *cUnit)
{
    disconnect(connectionUnit,SIGNAL(dataIn(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    disconnect(connectionUnit,SIGNAL(destroyed()),this,SLOT(detachConnection()));
    connectionUnit = cUnit;
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(connectionUnit,SIGNAL(destroyed()),this,SLOT(detachConnection()));
}

void LinkerUnit::detachConnection()
{
    connectionUnit = new ConnectionUnit;
}

void LinkerUnit::assignVariables(QList<ComplexVariable> newVariables)
{
    disconnect(connectionUnit,SIGNAL(dataIn(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    disconnect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
    parserEngine->setVariables(newVariables);
    parserEngine->clearVariables();
    results.clear();
    for(quint8 i=0;i<newVariables.size();i++)
    {
        ParsedVariable pv;
        results.append(pv);
    }
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
}

void LinkerUnit::parsedDataReady(VariableList parsedData)
{
    int complexCount=0;
    foreach(RepeatedVector repVector, parsedData)
    {
        if(repVector.vectors.size()<2)
        {
            // Single variable
            if(repVector.vectors.at(0).vector.at(0).varType==BYTTYPE)
            {
                results[complexCount].content = repVector.vectors.at(0).vector.at(0).varBytes;
            }
            else
            {
                // Number variable
                results[complexCount].content = repVector.vectors.at(0).vector.at(0).varBytes;
                results[complexCount].value = repVector.vectors.at(0).vector.at(0).varValue;
            }
        }
        complexCount++;
    }
    emit newDataPoint();
}
