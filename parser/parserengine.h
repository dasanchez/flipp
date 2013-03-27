#ifndef PARSERENGINE_H
#define PARSERENGINE_H

#include <QObject>
#include <QRegExp>
#include <QDebug>

#define BYTTYPE 0
#define NUMTYPE 1
#define VECTYPE 2

#define VALID_CHAR 0
#define INVALID_OK 1
#define INVALID_ERR 2

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

class SingleResult{
public:
    QByteArray varBytes;
    double varValue;
    int varType;
};

class SingleVector{
public:
    QList<SingleResult*> vector;
};

class RepeatedVector{
public:
    QList<SingleVector> vectors;
};

class ParserEngine : public QObject
{
    Q_OBJECT
public:
    explicit ParserEngine(QObject *parent = 0);
    void setVariables(QList<ComplexVariable*> *);
    bool isValid(QByteArray *);
signals:
    void dataParsed(QList<RepeatedVector>);
public slots:
    void parseData(QByteArray);

private:
    QList<ComplexVariable*> *targetVars;
    QList<RepeatedVector> masterList;
    QByteArray buffer;
    quint8 bufferCount;
    quint8 varIndex;
    quint8 matchIndex;
    quint8 vecIndex;
    quint8 repeatIndex;
    bool validList;
    QRegExp numRegex;
    // Look for an array of bytes that can contain +/-, spaces, and decimal points.


    // Private functions
    byteDecision checkByte(char);
    int assignNonNumber(char);
    int assignNumber(char);
    void variableComplete();
    void clearVariables();
    void resetVariables();
};

#endif // PARSERENGINE_H
