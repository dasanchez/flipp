#ifndef PARSERUNIT_H
#define PARSERUNIT_H

#include <QObject>
#include "../parser/parserengine.h"

class ParserUnit : public QObject
{
    Q_OBJECT
public:
    ParserUnit();
    QList<ComplexVariable> variableList;
    bool isValid();
    void setValid(bool);
    QString getName();
    void setName(QString);
signals:

public slots:
    void addVariable(ComplexVariable);
    void setVariableList(QList<ComplexVariable>);

private:
    bool validName;
    QString parserName;

};

#endif // PARSERUNIT_H
