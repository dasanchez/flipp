#ifndef PARSERENGINE_H
#define PARSERENGINE_H

#include <QObject>

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


class VariableClass {
public:
    QString name;
    quint8 type;
    bool fixed;
    bool match;
    quint8 length;
    quint8 repeat;
    QByteArray matchBytes;
    QList<QByteArray> received;
    QList<double> numValue;
};

class ParserEngine : public QObject
{
    Q_OBJECT
public:
    explicit ParserEngine(QObject *parent = 0);
    
signals:
    
public slots:

private:
    QList<VariableClass*> *variable;

};

#endif // PARSERENGINE_H
