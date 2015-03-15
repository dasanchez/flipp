#include "linkerunit.h"

LinkerUnit::LinkerUnit()
{
    connectionUnit = new ConnectionUnit;
    parserUnit = new ParserUnit;
    parserEngine = new ParserEngine;
    thread = new QThread;
    parserEngine->moveToThread(thread);
    parserEngine->setParser(true);  // WATCH OUT FOR THIS FLAG DURING OPERATION
    connect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));
    thread->start();
}

QString LinkerUnit::getConnectionName()
{
    return connectionUnit->getName();
}

void LinkerUnit::assignConnection(ConnectionUnit *cUnit)
{
    disconnect(connectionUnit,SIGNAL(dataIn(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    disconnect(connectionUnit,SIGNAL(destroyed()),this,SLOT(detachConnection()));
    connectionUnit = cUnit;
    name.clear();
    name.append(connectionUnit->getName());
    name.append(':');
    //name.append(parserUnit.getName());
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(connectionUnit,SIGNAL(destroyed()),this,SLOT(detachConnection()));
}

void LinkerUnit::detachConnection()
{
    connectionUnit = new ConnectionUnit;
}

void LinkerUnit::assignParser(ParserUnit *pUnit)
{
    disconnect(parserUnit,SIGNAL(variableListChanged()),this,SLOT(assignVariables()));
    parserUnit = pUnit;
    assignVariables(parserUnit->getList());
    connect(parserUnit,SIGNAL(variableListChanged()),this,SLOT(assignVariables()));
}

void LinkerUnit::assignVariables()
{
    assignVariables(parserUnit->getList());
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
    emit newVariableList();
    connect(connectionUnit,SIGNAL(dataIn(QByteArray)),parserEngine,SLOT(parseData(QByteArray)));
    connect(parserEngine,SIGNAL(dataParsed(VariableList)),this,SLOT(parsedDataReady(VariableList)));

}

QList<ComplexVariable> LinkerUnit::getVariables()
{
    return parserEngine->getVariables();
}

void LinkerUnit::parsedDataReady(VariableList parsedData)
{
    int complexCount=0;
    foreach(RepeatedVector repVector, parsedData)
    {
        if(repVector.vectors.size()<2)
        {
            results[complexCount].type = repVector.vectors.at(0).vector.at(0).varType;
            // Single variable
            if(results[complexCount].type==BYTTYPE)
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
