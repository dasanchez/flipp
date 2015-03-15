#ifndef PARSERUNIT_H
#define PARSERUNIT_H

#include <QObject>
#include "../parser/parserengine.h"

class ParserUnit : public QObject
{
    Q_OBJECT
public:
    ParserUnit();
    QList<ComplexVariable*> *variableList;
    bool hasValidName();
//    void setValid(bool);
    void setNameValid(bool);
    QString getName();

    QList<ComplexVariable> getList();
signals:
    void variableListChanged();
public slots:
    void addVariable(ComplexVariable*);
    void setVariableList(QList<ComplexVariable*>*);
    void setName(QString);
    void variablesUpdated();
private:
    bool validName;
    QString parserName;

};

#endif // PARSERUNIT_H
