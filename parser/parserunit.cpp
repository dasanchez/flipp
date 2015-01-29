#include "parserunit.h"

ParserUnit::ParserUnit()
{
    variableList.clear();
}

bool ParserUnit::isValid()
{
    return validName;
}

void ParserUnit::setValid(bool valid)
{
    validName = valid;
}

QString ParserUnit::getName()
{
    return parserName;
}

void ParserUnit::setName(QString newName)
{
    parserName = newName;
}

void ParserUnit::addVariable(ComplexVariable newVariable)
{
    variableList.append(newVariable);
}

void ParserUnit::setVariableList(QList<ComplexVariable> newList)
{
    variableList.clear();
    for(quint8 i=0;i<newList.size();i++)
    {
        variableList.append(newList.at(i));
    }
}
