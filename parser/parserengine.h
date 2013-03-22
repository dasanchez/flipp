#ifndef PARSERENGINE_H
#define PARSERENGINE_H

#include <QObject>
#include <QRegExp>
#include <QDebug>

#define BYTTYPE 0
#define NUMTYPE 1
#define VECTYPE 2

enum parsingstate{START,PARSE};
enum byteDecision{BYTE_HANDLED,BYTE_COMPLETES,BYTE_INVALID};

class BaseVariable{
public:
    QString name;
    quint8 type;
    bool fixed;
    bool match;
    quint8 length;
    QByteArray matchBytes;
};

class ComplexVariable{
public:
    QString name;
    quint8 type;
    bool fixed;
    bool match;
    quint8 length;
    QByteArray matchBytes;
    quint8 repeat;
    QList<BaseVariable*> *vector;
};

class ParsedValues{
public:
    QList<QByteArray*> bytesFound;
    QList<double*> valuesFound;
    QList<int*> varTypes;
};

class SingleResult{
public:
    QByteArray varBytes;
    double varValue;
    int varType;
};

class SingleVector{
public:
    QList<SingleResult*> *vectorResult;
};

class VectorReps{
public:
    QList<SingleVector*> *vectorReps;
};

class VectorListResult{
public:
    QList<QList<SingleResult> > *vectorList;
};




class ParserEngine : public QObject
{
    Q_OBJECT
public:
    explicit ParserEngine(QObject *parent = 0);
    
    void setVariables(QList<ComplexVariable*> *);
    bool isValid(QByteArray *);
signals:
    void dataParsed(QList<ParsedValues*> *);
public slots:
    void parseData(QByteArray);

private:
    QList<ComplexVariable*> *targetVars;
    QList<VectorReps*> *masterList;
//       QList<QList<SingleResult> > *vectorList;
//    QList<VectorListResult*> *parsedResult;
    QByteArray buffer;
    quint8 varIndex;
    quint8 matchIndex;
    quint8 vecIndex;
    quint8 repeatIndex;
    bool validList;

    // Private functions
    byteDecision checkByte(char);
    void assignNonNumber(char);
    void assignNumber(char);
    void variableComplete();
    void clearVariables();

    void resetVariables();
};

#endif // PARSERENGINE_H
