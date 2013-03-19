#include "parserengine.h"

ParserEngine::ParserEngine(QObject *parent) :
    QObject(parent)
{
    targetVars = new QList<ComplexVariable*>;
    resultVals = new QList<ParsedValues*>;
    varIndex=0;
    matchIndex =0;
    vecIndex=0;
    repeatIndex=0;
    validList=false;
}

void ParserEngine::setVariables(QList<ComplexVariable*> *newVars)
{
    targetVars = newVars;
    resetVariables();
}

void ParserEngine::parseData(QByteArray dataIn)
{
    if(!dataIn.isEmpty() && validList==true)
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
    bool handled=false;

    while(!handled)
    {
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
                handled=true;
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
                handled=true;
            }
            else
            {
                // Handle variable-length byte array
                if((onebyte>='0' && onebyte<='9') || onebyte=='+' || onebyte=='-' || onebyte=='.')
                {
                    // Assume number variable is now present.
                    //                qDebug() << *resultVals->at(varIndex)->bytesFound.at(0);
                    varIndex++;
                    //                    checkByte(onebyte);
                }
                else
                {
                    resultVals->at(varIndex)->bytesFound[0]->append(onebyte);
                    handled=true;
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
                handled=true;
            }
            else
            {
                // Variable-length number
                if(!((onebyte>='0' && onebyte<='9') || onebyte=='+' || onebyte=='-' || onebyte=='.'))
                {
                    // Assume byte variable is now present.
                    //                qDebug() << *resultVals->at(varIndex)->bytesFound.at(0);
                    double newVal = resultVals->at(varIndex)->bytesFound.at(0)->toDouble();
                    *resultVals->at(varIndex)->valuesFound[0]=newVal;
                    varIndex++;
                    //                    checkByte(onebyte);
                }
                else
                {
                    resultVals->at(varIndex)->bytesFound[0]->append(onebyte);
                    handled=true;
                }

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
        if (varIndex>=targetVars->size())
        {
            // Emit result
            emit dataParsed(resultVals);
            resetVariables();
        }
    }


    //    qDebug() << onebyte;


    return dec;
}

void ParserEngine::resetVariables()
{
    resultVals->clear();
    varIndex=0;
    matchIndex=0;
    vecIndex=0;
    repeatIndex =0;
    for(quint8 i=0;i<targetVars->size();i++)
    {
        ParsedValues *pvs = new ParsedValues;
        pvs->bytesFound.clear();
        pvs->valuesFound.clear();
        pvs->varTypes.clear();
        switch(targetVars->at(i)->type)
        {
        case VECTYPE:
            for(quint8 j=0;j<targetVars->at(i)->vector->size();j++)
            {
                // Assign properties for each BaseVariable
                pvs->varTypes.append(new int);
                pvs->bytesFound.append(new QByteArray);
                *pvs->varTypes[j]=targetVars->at(i)->vector->at(j)->type;
            }
            break;
        default:
            pvs->varTypes.append(new int);
            pvs->bytesFound.append(new QByteArray);
            pvs->valuesFound.append(new double);
            *pvs->varTypes[0]=targetVars->at(i)->type;
            break;
        }

        resultVals->append(pvs);
    }
}

bool ParserEngine::isValid(QByteArray *checkOutput)
{
    QByteArray testout;
    checkOutput->clear();
    if(targetVars->size()<2)
    {
        testout.append("Parser will only work with two or more variables");
        checkOutput->append(testout);
        validList=false;
        return validList;
    }


    // Check that for every variable-length byte array there is a number after it, or a matched byte array.
    for(quint8 i=0;i<targetVars->size();i++)
    {
        switch(targetVars->at(i)->type)
        {
        case BYTTYPE:
            // Invalid cases:
            // 1. Variable length in a middle position and the next variable is not a number or a matched byte.
            // 2. Variable length in the last position and the 1st variable is not a number or a matched byte.
            // 3. Variable length in a middle position and the next variable is a vector whose 1st item is not a number or a matched byte.
            // 4. Variable length in the last position and the 1st variable is a vector whose 1st item is not a number or a matched byte.
            // 5. Byte array to match is empty
            if(!(targetVars->at(i)->fixed || targetVars->at(i)->match))
            {
                if(i<targetVars->size()-1) // Have variables left.
                {
                    // Case 1
                    if(!(targetVars->at(i+1)->match || targetVars->at(i+1)->type==NUMTYPE))
                    {
                        testout.append("Use a number or matched-byte array in after a variable-length byte array");
                        checkOutput->append(testout);
                        validList=false;
                        return validList;
                    }
                    if(targetVars->at(i+1)->type==VECTYPE)
                    {
                        // Case 3
                        if(!(targetVars->at(i+1)->vector->at(0)->match || targetVars->at(i+1)->vector->at(0)->type==NUMTYPE))
                        {
                            testout.append("Use a number or matched-byte array in after a variable-length byte array");
                            checkOutput->append(testout);
                            validList=false;
                            return validList;
                        }
                    }
                }
                else
                {
                    // Case 2
                    if(!(targetVars->at(0)->match || targetVars->at(0)->type==NUMTYPE))
                    {
                        testout.append("Start with a number or matched byte array when the last variable is a variable-length byte array");
                        checkOutput->append(testout);
                        validList=false;
                        return validList;
                    }
                    if(targetVars->at(0)->type==VECTYPE)
                    {
                        // Case 4
                        if(!(targetVars->at(0)->vector->at(0)->match || targetVars->at(0)->vector->at(0)->type==NUMTYPE))
                        {
                            testout.append("Start with a number or matched byte array when the last variable is a variable-length byte array");
                            checkOutput->append(testout);
                            validList=false;
                            return validList;
                        }
                    }
                }
            }
            if(targetVars->at(i)->match && targetVars->at(i)->matchBytes.isEmpty())
            {
                // Case 5
                testout.append("Array to match is empty");
                checkOutput->append(testout);
                validList=false;
                return validList;
            }
            break;
        case NUMTYPE:
            // Invalid cases:
            // 1. Variable length in a middle position and the next variable is also a number.
            // 2. Variable length in a middle position and the next variable is a vector whose first item is also a number.
            // 3. Variable length in the last position and the 1st variable is also a number.
            // 4. Variable length in the last position and the 1st variable is a vector whose first item is also a number.

            if(!targetVars->at(i)->fixed)
            {
                if(i<targetVars->size()-1)
                {
                    // Case 1
                    if(targetVars->at(i+1)->type==NUMTYPE)
                    {
                        testout.append("Use a byte array after a variable-length number");
                        checkOutput->append(testout);
                        validList=false;
                        return validList;
                    }
                    // Case 2
                    if(targetVars->at(i+1)->type==VECTYPE && targetVars->at(i+1)->vector->at(0)->type==NUMTYPE)
                    {
                        testout.append("Use a byte array after a variable-length number");
                        checkOutput->append(testout);
                        validList=false;
                        return validList;
                    }
                }
                else
                {
                    // Case 3
                    if(targetVars->at(0)->type==NUMTYPE)
                    {
                        testout.append("Start with a byte array if the last variable is a variable-length number");
                        checkOutput->append(testout);
                        validList=false;
                        return validList;
                    }
                    // Case 4
                    if(targetVars->at(0)->type==VECTYPE && targetVars->at(0)->vector->at(0)->type==NUMTYPE)
                    {
                        testout.append("Start with a byte array if the last variable is a variable-length number");
                        checkOutput->append(testout);
                        validList=false;
                        return validList;
                    }
                }
            }

            break;
        case VECTYPE:
            for(quint8 j=0;j<targetVars->at(i)->vector->size();j++)
            {
                switch(targetVars->at(i)->vector->at(j)->type)
                {
                case BYTTYPE:
                    // Invalid cases:
                    // 1. Variable length in a middle vector position and the next vector item is not a number or a matched byte.
                    // 2. Variable length in the last vector position and the 1st vector item is not a number or a matched byte.
                    // 3. Variable length in the last vector position and the next variable is not a number or a matched byte.
                    // 4. Variable length in the last position and the 1st variable is not a number or a matched byte.
                    // 5. Byte array to match is empty
                    if(j<targetVars->at(i)->vector->size()-1)
                    {
                        // Case 1
                        if(!(targetVars->at(i)->vector->at(j+1)->match || targetVars->at(i)->vector->at(j+1)->type==NUMTYPE))
                        {
                            testout.append("Use a number or matched-byte array in after a variable-length byte array");
                            checkOutput->append(testout);
                            validList=false;
                            return validList;
                        }
                    }
                    else
                    {
                        // Case 2
                        if(!(targetVars->at(i)->vector->at(0)->match || targetVars->at(i+1)->vector->at(0)->type==NUMTYPE))
                        {
                            testout.append("Start with a number or matched byte array when the last variable is a variable-length byte array");
                            checkOutput->append(testout);
                            validList=false;
                            return validList;
                        }
                        if(i<targetVars->size()-1)
                        {
                            // Case 3
                            if(!(targetVars->at(i+1)->match || targetVars->at(i+1)->type==NUMTYPE))
                            {
                                testout.append("Use a number or matched-byte array in after a variable-length byte array");
                                checkOutput->append(testout);
                                validList=false;
                                return validList;
                            }
                        }
                    }

                    break;
                case NUMTYPE:
                    // Invalid cases:
                    // 1. Variable length in a middle vector position and the next variable is a number.
                    // 2. Variable length in the last vector position and the 1st vector item is also a number.
                    // 3. Variable length in the last vector position and the next variable is also a number.
                    // 4. Variable length in the last vector position and the 1st variable is also a number.
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }

        //        if(targetVars->at(j)->type==NUMTYPE)
        //        {
        //          varList[j].floatvalue=0;
        //        }

        //        if(targetVars->at(j)->type==BYTTYPE && !(targetVars->at(j)->fixed || targetVars->at(j)->match))
        //        { // Variable length array, not matched
        //            if(j<targetVars->size()-1) // Still have variables left
        //            {
        //                if(!(targetVars->at(j+1)->match || targetVars->at(j+1)->type==NUMTYPE))  // Next variable is not matched, and it's not a number either
        //                {
        //                    testout.append("Use a number or matched-byte array in front of a variable-length byte array");
        //                    validList=false;
        //                    return validList;
        //                }
        //            }
        //            else // end of the vector
        //            {
        //                if(!(targetVars->at(0)->match || targetVars->at(0)->type==NUMTYPE))
        //                {
        //                    testout.append("Start with a number or matched byte array when the last variable is a variable-length byte array");
        //                    validList=false;
        //                    return validList;
        //                }
        //            }
        //        }
        //        if(targetVars->at(j)->type==NUMTYPE && !(targetVars->at(j)->fixed))  // do not put another number variable right after a variable-length number
        //        {
        //            if(j<targetVars->size()-1) // Still have variables left
        //            {
        //                if(targetVars->at(j+1)->type!=BYTTYPE) // next variable is a number
        //                {
        //                    testout.append("Use a byte array in front of a variable-length number");
        //                    validList=false;
        //                    return validList;
        //                }
        //            }
        //            else
        //            {
        //                if(targetVars->at(0)->type!=BYTTYPE)  // no variables left, check first variable for byte type
        //                {
        //                    testout.append("Start with a byte array if the last variable is a variable-length number");
        //                    validList=false;
        //                    return validList;
        //                }
        //            }

        //        }
        //        if(targetVars->at(j)->type==BYTTYPE && targetVars->at(j)->match)
        //        {
        //            // Do not accept matched byte if array to match is empty
        //            if(targetVars->at(j)->matchBytes.isEmpty())
        //            {
        //                testout.append("Array to match is empty");
        //                validList=false;
        //                return validList;
        //            }
        //        }
    }

    validList=true;
    testout.append("Variable list is valid");
    //      curstate=START;
    //      clearVarList();

    return validList;
}
