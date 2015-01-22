#include "parsingunit.h"

ParsingUnit::ParsingUnit(QObject *parent) :
    QObject(parent)
{
    parserEngine = new ParserEngine;

}
