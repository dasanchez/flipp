#include "parserengine.h"

ParserEngine::ParserEngine(QObject *parent) :
    QObject(parent)
{
    targetVars = new QList<ComplexVariable*>;
    resultVals = new QList<ParsedValues*>;
    varIndex=0;
    matchIndex =0;
    vecIndex=0;
}

void ParserEngine::setVariables(QList<ComplexVariable*> *newVars)
{
    targetVars = newVars;
    resetVariables();
}

void ParserEngine::parseData(QByteArray dataIn)
{
    if(!dataIn.isEmpty())
    {
        foreach(char ch,dataIn)
        {
            //            qDebug() << ch;
            checkByte(ch);
        }
    }
}

// CheckByte receives a single byte, and allocates it to the corresponding variable.
// There are two special cases of byte processing:

// 1. The current variable is a variable-length byte array and a number is received.
// If the number can be allocated to the next variable (or variable #0 if the current
// byte array is the last element in the list), the current byte array is closed.
// Otherwise the function returns INVALID_MATCH

// 2. The current variable is a variable-length number and a non-valid byte is received.
// If the byte can be allocated to the next variable (or variable #0 if the current number
// is the last element in the list), the current number is closed.

byteDecision ParserEngine::checkByte(char onebyte)
{

    byteDecision dec=BYTE_HANDLED;
    QRegExp regexp;
    // Look for an array of bytes that can contain +/-, spaces, and decimal points.
    regexp.setPattern("( *[-+]? *\\d+\\.?\\d*)|( *[-+]? *\\d*\\.?\\d+)| +| *[+-]| *[+-] +| *[+-]? *\\.");

    ComplexVariable *currentVar = targetVars->at(varIndex);
    switch(currentVar->type)
    {
    case BYTTYPE:
        if(currentVar->match)
        {
            // Matched byte array
            // Will the next byte fit in?
            if(currentVar->matchBytes.at(matchIndex)==onebyte)
            {
                resultVals->at(varIndex)->bytesFound[0]->append(onebyte);
                if(currentVar->matchBytes.size()==resultVals->at(varIndex)->bytesFound.at(0)->size())
                {
//                    qDebug() << *resultVals->at(varIndex)->bytesFound.at(0);
                    matchIndex=0;
                    varIndex++;
                }
                else
                {
                    matchIndex++;
                }

            }
            else
            {
                resultVals->at(varIndex)->bytesFound[0]->clear();
                varIndex=0;
                matchIndex=0;
            }
        }
        else if(currentVar->fixed)
        {
            // Fixed-length byte array
            resultVals->at(varIndex)->bytesFound[0]->append(onebyte);
            if(currentVar->length==resultVals->at(varIndex)->bytesFound.at(0)->size())
            {
//                qDebug() << *resultVals->at(varIndex)->bytesFound.at(0);
                varIndex++;
            }
        }
        else
        {
            // Handle variable-length byte array
            if((onebyte>='0' && onebyte<='9') || onebyte=='+' || onebyte=='-' || onebyte=='.')
            {
                // Assume number variable is now present.
//                qDebug() << *resultVals->at(varIndex)->bytesFound.at(0);
                varIndex++;
                checkByte(onebyte);
            }
            else
            {
                resultVals->at(varIndex)->bytesFound[0]->append(onebyte);
            }
        }
        break;
    case NUMTYPE:
        if(currentVar->fixed)
        {
            // Fixed-length number
            resultVals->at(varIndex)->bytesFound[0]->append(onebyte);
            if(currentVar->length==resultVals->at(varIndex)->bytesFound.at(0)->size())
            {
                double newVal = resultVals->at(varIndex)->bytesFound.at(0)->toDouble();
                *resultVals->at(varIndex)->valuesFound[0]=newVal;
//                qDebug() << *resultVals->at(varIndex)->bytesFound.at(0) << ", number: " << newVal;
                varIndex++;
            }
        }
        else
        {
            // Variable-length number

        }
        break;
    case VECTYPE:
        // We'll get here.
        BaseVariable *currentVectorItem;
        currentVectorItem = currentVar->vector->at(vecIndex);
        switch(currentVectorItem->type)
        {
        case BYTTYPE:
            if(currentVectorItem->match)
            {

            }
            else if(currentVectorItem->fixed)
            {

            }
            else
            {

            }
            break;
        case NUMTYPE:
            if(currentVectorItem->fixed)
            {

            }
            else
            {

            }
            break;
        default:
            break;
        }

        break;
    default:
        break;
    }


    //    qDebug() << onebyte;
    if (varIndex>=targetVars->size())
    {
        // Emit result
        emit dataParsed(resultVals);
        resetVariables();
    }

    return dec;
}

void ParserEngine::resetVariables()
{
    resultVals->clear();
    varIndex=0;
    matchIndex=0;
    vecIndex=0;
    for(quint8 i;i<targetVars->size();i++)
    {
        ParsedValues *pvs = new ParsedValues;
        pvs->bytesFound.clear();
        pvs->bytesFound.append(new QByteArray);
        pvs->valuesFound.clear();
        pvs->valuesFound.append(new double);
        pvs->varTypes.clear();
        pvs->varTypes.append(new int);
        switch(targetVars->at(i)->type)
        {
        case BYTTYPE:
            *pvs->varTypes[0]=BYTTYPE;
            break;
        case NUMTYPE:
            *pvs->varTypes[0]=NUMTYPE;
            break;
        default:
            break;
        }

        resultVals->append(pvs);

    }
}
