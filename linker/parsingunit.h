#ifndef PARSINGUNIT_H
#define PARSINGUNIT_H

#include <QObject>
#include <QThread>
#include "../parser/parserengine.h"
#include "../connection/connectionunit.h"

class ParsingUnit : public QObject
{
    Q_OBJECT
public:
    explicit ParsingUnit(QObject *parent = 0);

signals:

public slots:

private:
    ParserEngine *parserEngine;

};

#endif // PARSINGUNIT_H
