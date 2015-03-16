#include "parserunit.h"

ParserUnit::ParserUnit()
{
    variableList = new QList<ComplexVariable*>;
    parserName.clear();
}

bool ParserUnit::hasValidName()
{
    return validName;
}

//void ParserUnit::setValid(bool valid)
//{
//    validName = valid;
//}

void ParserUnit::setNameValid(bool valid)
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

void ParserUnit::variablesUpdated()
{
    emit variableListChanged();
}

void ParserUnit::addVariable(ComplexVariable *newVariable)
{
    variableList->append(newVariable);
    emit variableListChanged();
}

QList<ComplexVariable> ParserUnit::getList()
{
    QList<ComplexVariable> vList;
    for(quint8 i=0;i<variableList->size();i++)
    {
        ComplexVariable cVar;
        cVar.name = variableList->at(i)->name;
        cVar.type = variableList->at(i)->type;
        cVar.fixed = variableList->at(i)->fixed;
        cVar.length = variableList->at(i)->length;
        cVar.match = variableList->at(i)->match;
        cVar.matchBytes = variableList->at(i)->matchBytes;
        vList.append(cVar);
    }
    return vList;
}

void ParserUnit::setVariableList(QList<ComplexVariable*> *newList)
{
    variableList = newList;
//    emit variableListChanged();
//    variableList.clear();
//    for(quint8 i=0;i<newList.size();i++)
//    {
//        variableList.append(newList.at(i));
//    }
}
