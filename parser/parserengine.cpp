#include "parserengine.h"

ParserEngine::ParserEngine(QObject *parent) :
    QObject(parent)
{
    targetVars = new QList<ComplexVariable*>;
    buffer.clear();
    bufferCount=0;
    varIndex=0;
    matchIndex =0;
    vecIndex=0;
    repeatIndex=0;
    validList=false;
    listComplete = false;

    // Look for an array of bytes that can contain +/-, spaces, and decimal points.
    numRegex.setPattern("( *[-+]? *\\d+\\.?\\d*)|( *[-+]? *\\d*\\.?\\d+)| +| *[+-]| *[+-] +| *[+-]? *\\.");
}

void ParserEngine::setVariables(QList<ComplexVariable*> *newVars)
{
    targetVars = newVars;
    resetVariables();
}

void ParserEngine::parseData(QByteArray dataIn)
{
    quint16 i=0;
    if(!dataIn.isEmpty() && validList==true)
    {
         while(dataIn.size()>0)
        {
             // Loop while there is data in the incoming buffer.
            switch(checkByte(dataIn.at(i)))
            {
            case BYTE_INVALID:
                // Eliminate left-most byte from buffer and try again.
                dataIn = dataIn.right(dataIn.size()-1);
                i=0;
                break;
            default:
                // The new byte was handled without any problems.
                if(listComplete)
                {
                    // The listComplete flag is true, so the part of the buffer
                    // known to be fully parsed is removed.
                    listComplete=false;
                    dataIn = dataIn.right(dataIn.size()-i-1);
                    i=0;
                }
                else
                {
                    // Move to the next byte in the buffer. Finish looping
                    // if the end of the buffer is reached.
                    i++;
                    if(i==dataIn.size())
                        dataIn.clear();
                }
                break;
            }
        }

    }
}

// CheckByte receives a single byte, and allocates it to the corresponding variable.
byteDecision ParserEngine::checkByte(char onebyte)
{

    byteDecision dec=BYTE_HANDLED;

    if(targetVars->at(varIndex)->type!=VECTYPE)
    {
        if(targetVars->at(varIndex)->type==BYTTYPE)
        {
            // Assign non-number to array
            // Possible responses:
            // 1. Byte was handled without errors.
            // 2. Byte was demeed invalid, try next variable
            // 3. Byte was deemed invalid, eliminate current buffer
            switch(assignNonNumber(onebyte))
            {
            case VALID_CHAR:
                // Case 1
                return BYTE_HANDLED;
                break;
            case INVALID_OK:
                // Case 2
                // A number was found. Is next variable/vector item a number?
                switch(assignNumber(onebyte))
                {
                case VALID_CHAR:
                    return BYTE_HANDLED;
                    break;
                case INVALID_OK:
                case INVALID_ERR:
                default:
                    clearVariables();
                    return BYTE_INVALID;
                    break;
                }
                break;
            case INVALID_ERR:
                // Case 3
                clearVariables();
                return BYTE_INVALID;
                break;
            default:
                break;
            }
        }
        else
        {
            // Assign number
            // Possible responses:
            // 1. Number has handled without errors.
            // 2. Number was deemed invalid, try next variable
            // 3. Number was deemed invalid, eliminate current buffer
            switch(assignNumber(onebyte))
            {
            case VALID_CHAR:
                // Case 1
                return BYTE_HANDLED;
                break;
            case INVALID_OK:
                // Case 2
                // A number character was found. Can it be assigned to the next variable?
                switch(assignNonNumber(onebyte))
                {
                case VALID_CHAR:
                    return BYTE_HANDLED;
                    break;
                default:
                    clearVariables();
                    return BYTE_INVALID;
                    break;
                }
                break;
            case INVALID_ERR:
                // Case 3
                clearVariables();
                return BYTE_INVALID;
                break;
            default:
                break;
            }
        }
    }
    else
    {
        if(targetVars->at(varIndex)->vector->at(vecIndex)->type==BYTTYPE)
        {
            // Assign non-number to array
            // Possible responses:
            // 1. Byte was handled without errors.
            // 2. Byte was demeed invalid, try next variable
            // 3. Byte was deemed invalid, eliminate current buffer
            switch(assignNonNumber(onebyte))
            {
            case VALID_CHAR:
                // Case 1
                return BYTE_HANDLED;
                break;
            case INVALID_OK:
                // Case 2
                // A number was found. Is next variable/vector item a number?
                switch(assignNumber(onebyte))
                {
                case VALID_CHAR:
                    return BYTE_HANDLED;
                    break;
                default:
                    clearVariables();
                    return BYTE_INVALID;
                    break;
                }
                break;
            case INVALID_ERR:
                // Case 3
                clearVariables();
                return BYTE_INVALID;
                break;
            default:
                break;
            }
        }
        else
        {
            // Number type
            // Possible responses:
            // 1. Number has handled without errors.
            // 2. Number was deemed invalid, try next variable
            // 3. Number was deemed invalid, eliminate current buffer
            switch(assignNumber(onebyte))
            {
            case VALID_CHAR:
                // Case 1
                return BYTE_HANDLED;
                break;
            case INVALID_OK:
                switch(assignNonNumber(onebyte))
                {
                case VALID_CHAR:
                    return BYTE_HANDLED;
                    break;
                default:
                    clearVariables();
                    return BYTE_INVALID;
                    break;
                }
                // Case 2
                break;
            case INVALID_ERR:
                // Case 3
                clearVariables();
                return BYTE_INVALID;
                break;
            default:
                break;
            }
        }
    }

    return dec;
}

void ParserEngine::resetVariables()
{
    buffer.clear();
    masterList.clear();
    varIndex=0;
    matchIndex=0;
    vecIndex=0;
    repeatIndex =0;
    for(quint8 i=0;i<targetVars->size();i++)
    {
        RepeatedVector repVec;
        // Populate vector list result with data containers
        switch(targetVars->at(i)->type)
        {
        case VECTYPE:
        {
            // Iterate through each repetition
            for(quint8 j=0;j<targetVars->at(i)->repeat;j++)
            {
                // Iterate through each vector element
                SingleVector sv;
                for(quint8 k=0;k<targetVars->at(i)->vector->size();k++)
                {
                    SingleResult *sr = new SingleResult;
                    sr->varType=targetVars->at(i)->vector->at(k)->type;
                    sv.vector.append(sr);
                }
                repVec.vectors.append(sv);
            }
        }
            break;
        default:

            SingleVector sv;

            SingleResult *sr = new SingleResult;
            sr->varType=targetVars->at(i)->type;
            sv.vector.append(sr);
            repVec.vectors.append(sv);
            break;
        }
        masterList.append(repVec);
    }

}

bool ParserEngine::isValid(QByteArray *checkOutput)
{
    QByteArray testout;
    checkOutput->clear();

    // Check list size
    if(targetVars->size()<2)
    {
        testout.append("Parser will only work with two or more variables");
        checkOutput->append(testout);
        validList=false;
        return validList;
    }

    // Cycle through each variable
    for(quint8 i=0;i<targetVars->size();i++)
    {
        // Make sure vectors are not empty
        if(targetVars->at(i)->type==VECTYPE && targetVars->at(i)->vector->isEmpty())
        {
            testout.append("Vector is empty.");
            checkOutput->append(testout);
            validList=false;
            return validList;
        }

        // Check all special cases
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
                    // 6. Variable length in the last vector position and the next variable is a vector whose first item is not a number or matched byte.
                    // 7. Variable length in the last position and the 1st variable is a vector whose first item is not a number or matched byte.

                    if(!(targetVars->at(i)->vector->at(j)->fixed || targetVars->at(i)->vector->at(j)->match))
                    {
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

                                if(targetVars->at(i+1)->type==VECTYPE)
                                {
                                    // Case 6
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
                                // Case 4
                                if(!(targetVars->at(0)->match || targetVars->at(0)->type==NUMTYPE))
                                {
                                    testout.append("Start with a number or matched byte array when the last variable is a variable-length byte array");
                                    checkOutput->append(testout);
                                    validList=false;
                                    return validList;
                                }
                                if(targetVars->at(0)->type==VECTYPE)
                                {
                                    // Case 7
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
                        // Case 5
                        if(targetVars->at(i)->vector->at(j)->match && targetVars->at(i)->vector->at(j)->matchBytes.isEmpty())
                        {
                            testout.append("Array to match is empty");
                            checkOutput->append(testout);
                            validList=false;
                            return validList;
                        }
                    }
                    break;
                case NUMTYPE:
                    // Invalid cases:
                    // 1. Variable length in a middle vector position and the next variable is a number.
                    // 2. Variable length in the last vector position and the 1st vector item is also a number.
                    // 3. Variable length in the last vector position and the next variable is also a number.
                    // 4. Variable length in the last vector position and the 1st variable is also a number.
                    // 5. Variable length in the last vector position and the next variable is a vector whose first item is also a number.
                    // 6. Variable length in the last position and the 1st variable is a vector whose first item is also a number.
                    if(!targetVars->at(i)->vector->at(j)->fixed)
                    {
                        if(j<targetVars->at(i)->vector->size()-1)
                        {
                            // Case 1
                            if(!(targetVars->at(i)->vector->at(j+1)->match || targetVars->at(i)->vector->at(j+1)->type==NUMTYPE))
                            {
                                testout.append("Use a byte array after a variable-length number");
                                checkOutput->append(testout);
                                validList=false;
                                return validList;
                            }
                        }
                        else
                        {
                            // Case 2
                            if(targetVars->at(i)->vector->at(0)->type==NUMTYPE)
                            {
                                testout.append("Start with a byte array if the last variable is a variable-length number");
                                checkOutput->append(testout);
                                validList=false;
                                return validList;
                            }
                            // Case 3
                            if(i<targetVars->size()-1)
                            {
                                if(targetVars->at(i+1)->type==NUMTYPE)
                                {
                                    testout.append("Use a byte array after a variable-length number");
                                    checkOutput->append(testout);
                                    validList=false;
                                    return validList;
                                }
                                if(targetVars->at(i+1)->type==VECTYPE)
                                {
                                    // Case 5
                                    if(targetVars->at(i+1)->vector->at(0)->type==NUMTYPE)
                                    {
                                        testout.append("Use a byte array after a variable-length number");
                                        checkOutput->append(testout);
                                        validList=false;
                                        return validList;
                                    }
                                }
                            }
                            else
                            {
                                // Case 4
                                if(targetVars->at(0)->type==NUMTYPE)
                                {
                                    testout.append("Start with a byte array if the last variable is a variable-length number");
                                    checkOutput->append(testout);
                                    validList=false;
                                    return validList;
                                }
                                // Case 6
                                if(targetVars->at(0)->vector->at(0)->type==NUMTYPE)
                                {
                                    testout.append("Start with a byte array if the last variable is a variable-length number");
                                    checkOutput->append(testout);
                                    validList=false;
                                    return validList;
                                }
                            }
                        }
                    }

                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }

    }

    validList=true;
    testout.append("Variable list is valid");
    checkOutput->append(testout);
    return validList;
}

int ParserEngine::assignNonNumber(char newChar)
{
    // Handle match:
    // Do ComplexVariable first
    if(targetVars->at(varIndex)->type!=VECTYPE)
    {
        if(targetVars->at(varIndex)->match)
        {

            if(targetVars->at(varIndex)->matchBytes.at(matchIndex)==newChar)
            {
                // Byte matches OK
                // Two cases:
                // 1. Byte completes matchBytes array
                // 2. Byte does not complete matchBytes array (default)
                masterList.at(varIndex).vectors[0].vector[0]->varBytes.append(newChar);
                matchIndex++;
                if(masterList.at(varIndex).vectors[0].vector[0]->varBytes.size()==targetVars->at(varIndex)->matchBytes.size())
                {
                    // Case 1
                    matchIndex=0;
                    variableComplete();
                }

                return VALID_CHAR;
            }
            else
            {
                // Incoming byte does not match target array
                return INVALID_ERR;
            }
        }
        else if(targetVars->at(varIndex)->fixed)
        {
            // Fixed
            // Two cases:
            // 1. Byte completes variable length
            // 2. Byte does not complete variable length
            masterList.at(varIndex).vectors[0].vector[0]->varBytes.append(newChar);
            if(masterList.at(varIndex).vectors[0].vector[0]->varBytes.size()==targetVars->at(varIndex)->length)
            {
                // Case 1
                variableComplete();
            }
            return VALID_CHAR;
        }
        else
        {
            // Variable length
            // Case 1: Incoming byte can be used as a number character: [+ - . (0-9)]
            // Case 2: Incoming byte can not be used as a number character
            switch (newChar)
            {
            // Case 1:
            case '+':
            case '-':
            case '.':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                variableComplete();
                return INVALID_OK;
                break;
            default:
                // Case 2:
                masterList.at(varIndex).vectors[0].vector[0]->varBytes.append(newChar);
                break;
            }
            return VALID_CHAR;
        }
    }
    // Do BaseVariable next
    else
    {
        if(targetVars->at(varIndex)->vector->at(vecIndex)->match)
        {
            // Matched bytes
            if(targetVars->at(varIndex)->vector->at(vecIndex)->matchBytes.at(matchIndex)==newChar)
            {
                // byte matches OK
                // Two cases:
                // 1. Byte completes matchBytes array
                // 2. Byte does not complete matchBytes array (default)
                masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.append(newChar);
                matchIndex++;
                if(masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.size()==targetVars->at(varIndex)->vector->at(vecIndex)->matchBytes.size())
                {
                    // Case 1
                    matchIndex=0;
                    variableComplete();
                }
                return VALID_CHAR;
            }
            else
            {
                // Incoming byte does not match target array
                return INVALID_ERR;
            }
        }
        else  if(targetVars->at(varIndex)->vector->at(vecIndex)->fixed)
        {
            // Fixed
            // Two cases:
            // 1. Byte completes variable length
            // 2. Byte does not complete variable length
            masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.append(newChar);
            if(masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.size()==targetVars->at(varIndex)->vector->at(vecIndex)->length)
            {
                // Case 1
                variableComplete();
            }
            return VALID_CHAR;
        }
        else
        {
            // Variable length
            // Case 1: Incoming byte can be used as a number character: [+ - . (0-9)]
            // Case 2: Incoming byte can not be used as a number character
            switch (newChar)
            {
            // Case 1:
            case '+':
            case '-':
            case '.':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                variableComplete();
                return INVALID_OK;
                break;
            default:
                // Case 2:
                masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.append(newChar);
                break;
            }
            return VALID_CHAR;
        }
    }
}

int ParserEngine::assignNumber(char newChar)
{
    // 1st check: is it a number-friendly byte?
    switch(newChar)
    {
    case ' ':
    case '+':
    case '-':
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        // Incoming byte is a number
        // Cases:
        // Complex Variable
        if(targetVars->at(varIndex)->type!=VECTYPE)
        {

            if(targetVars->at(varIndex)->fixed)
            {
                // Fixed length
                // Case 1: Incoming byte completes length
                // Case 2: Incoming byte does not complete length
                // Case 3: Incoming byte invalidates buffer
                masterList.at(varIndex).vectors[0].vector[0]->varBytes.append(newChar);
                if(masterList.at(varIndex).vectors[0].vector[0]->varBytes.size()==targetVars->at(varIndex)->length)
                {
                    // Case 1
                    masterList.at(varIndex).vectors[0].vector[0]->varValue=masterList.at(varIndex).vectors[0].vector[0]->varBytes.toDouble();
                    variableComplete();
                }
                return VALID_CHAR;
            }
            else
            {
                // Variable length
                // Case 1: Incoming byte keeps buffer valid
                // Case 2: Incoming byte invalidates buffer
                QByteArray checkArray;
                checkArray.append(masterList.at(varIndex).vectors[0].vector[0]->varBytes);
                checkArray.append(newChar);
                if(numRegex.indexIn(checkArray)==0 && numRegex.matchedLength()==checkArray.size())
                {
                    // Case 1
                    masterList.at(varIndex).vectors[0].vector[0]->varBytes.append(newChar);
                    return VALID_CHAR;
                }
                else
                {
                    // Case 2
                    masterList.at(varIndex).vectors[0].vector[0]->varValue=masterList.at(varIndex).vectors[0].vector[0]->varBytes.toDouble();
                    variableComplete();
                    return INVALID_OK;
                }
            }

        }
        // Base Variable
        else
        {
            if(targetVars->at(varIndex)->vector->at(vecIndex)->fixed)
            {
                // Fixed length
                // Case 1: Incoming byte completes length
                // Case 2: Incoming byte does not complete length
                // Case 3: Incoming byte invalidates buffer
                masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.append(newChar);
                if(masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.size()==targetVars->at(varIndex)->vector->at(vecIndex)->length)
                {
                    // Case 1
                    masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varValue=masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.toDouble();
                    variableComplete();
                }
                return VALID_CHAR;
            }
            else
            {
                // Variable length
                // Case 1: Incoming byte keeps buffer valid
                // Case 2: Incoming byte invalidates buffer
                QByteArray checkArray;
                checkArray.append(masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes);
                checkArray.append(newChar);
                if(numRegex.indexIn(checkArray)==0 && numRegex.matchedLength()==checkArray.size())
                {
                    // Case 1
                    masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.append(newChar);
                    return VALID_CHAR;
                }
                else
                {
                    // Case 2
                    masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varValue=masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.toDouble();
                    variableComplete();
                    return INVALID_OK;
                }
            }
        }
        break;
    default:
        // Incoming byte is not even a number
        // Check Complex Variable first
        if(targetVars->at(varIndex)->type!=VECTYPE)
        {
            if(targetVars->at(varIndex)->fixed)
            {
                return INVALID_ERR;
            }
            else
            {
                // Variable-length
                // Case 1: Incoming byte keeps buffer valid
                // Case 2: Incoming byte invalidates buffer
                QByteArray checkArray;
                checkArray.append(masterList.at(varIndex).vectors[0].vector[0]->varBytes);
                checkArray.append(newChar);
                if(numRegex.indexIn(checkArray)==0 && numRegex.matchedLength()==checkArray.size())
                {
                    // Case 1
                    // Modify to check that the matched regex length equals the length in checkarray
                    masterList.at(varIndex).vectors[0].vector[0]->varBytes.append(newChar);
                    return VALID_CHAR;
                }
                else
                {
                    // Case 2
                    masterList.at(varIndex).vectors[0].vector[0]->varValue=masterList.at(varIndex).vectors[0].vector[0]->varBytes.toDouble();
                    variableComplete();
                    return INVALID_OK;
                }
            }
        }
        else
        {
            // Base Variable
            if(targetVars->at(varIndex)->vector->at(vecIndex)->fixed)
            {
                return INVALID_ERR;
            }
            else
            {
                // Variable-length
                // Case 1: Incoming byte keeps buffer valid
                // Case 2: Incoming byte invalidates buffer
                QByteArray checkArray;
                checkArray.append(masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes);
                checkArray.append(newChar);

                if(numRegex.indexIn(checkArray)==0 && numRegex.matchedLength()==checkArray.size())
                {
                    // Case 1
                    masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.append(newChar);
                    return VALID_CHAR;
                }
                else
                {
                    // Case 2
                    masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varValue=masterList.at(varIndex).vectors[repeatIndex].vector[vecIndex]->varBytes.toDouble();
                    variableComplete();
                    return INVALID_OK;
                }
            }
        }
        break;
    }
}

void ParserEngine::variableComplete()
{
    // ComplexVariable case:
    if(targetVars->at(varIndex)->type==VECTYPE)
    {
        vecIndex++;
        if(vecIndex==targetVars->at(varIndex)->vector->size())
        {
            // Vector is complete
            repeatIndex++;
            vecIndex=0;
            if(repeatIndex==targetVars->at(varIndex)->repeat)
            {
                // All repetitions have been caught.
                varIndex++;
                repeatIndex=0;
            }
        }
        else
        {
            // Vector is not complete yet
            return;
        }
    }
    else
    {
        // For BYTTYPE and NUMTYPE, always increase variable index
        varIndex++;
    }
    if(varIndex==targetVars->size())
    {
        listComplete=true;
        emit dataParsed(masterList);
//        qDebug() << "Full list caught";
        clearVariables();

    }
}

void ParserEngine::clearVariables()
{
    //    resultVals->clear();
    varIndex=0;
    matchIndex=0;
    vecIndex=0;
    repeatIndex =0;
    for(quint8 i=0;i<masterList.size();i++)
    {
        for(quint8 j=0;j<masterList.at(i).vectors.size();j++)
        {
            for(quint8 k=0;k<masterList.at(i).vectors.at(j).vector.size();k++)
            {
                masterList.at(i).vectors.at(j).vector.at(k)->varBytes.clear();
                masterList.at(i).vectors.at(j).vector.at(k)->varValue=0;
                //                resultVals->at(i)->bytesFound.at(j)->clear();
                //                resultVals->at(i)->valuesFound.at(j)->clear();
            }
        }
    }
}
