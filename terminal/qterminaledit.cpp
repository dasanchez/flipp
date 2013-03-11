#include "qterminaledit.h"

QTerminalEdit::QTerminalEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    echoBytes = false;
    hexMode = false;
    setFont(QFont("Consolas"));
    setWordWrapMode(QTextOption::WrapAnywhere);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMaximumBlockCount(100);
}

void QTerminalEdit::keyPressEvent(QKeyEvent *e)
{
   QByteArray dataIn;
   dataIn.append(e->text());
    if(echoBytes)
    {
        appendText(dataIn,true);
    }
    emit textEntered(e->text(),hexMode);
}

void QTerminalEdit::toggleEcho()
{
    echoBytes = !echoBytes;
}

bool QTerminalEdit::willEcho(void)
{
    return echoBytes;
}

void QTerminalEdit::setHex(bool hex)
{
    hexMode = hex;
    setWordWrapMode(QTextOption::WordWrap);
}

void QTerminalEdit::appendText(QByteArray newText)
{
    QTextCursor tc = this->textCursor();
    QTextCharFormat tcf = tc.charFormat();
    tcf.setForeground(QBrush(QColor("white")));
    tc.movePosition(QTextCursor::End);
    tc.setCharFormat(tcf);

    if(hexMode)
    {
        QString hexFormat;

        while(newText.size()>0)
        {
            QString temp = newText.left(1).toHex().toUpper();
            newText.remove(0,1);
            hexFormat.append(temp);
            hexFormat.append(" ");
            if(temp==QString("0D"))
            {
                hexFormat.append('\n');
            }
        }

        tc.insertText(hexFormat);
        tc.movePosition(QTextCursor::End);
    }
    else
    {
        QString data;
        foreach(char ch, newText)
        {
            if(ch !=0)
            {
                data.append(ch);
            }
        }

        tc.insertText(data);
        tc.movePosition(QTextCursor::End);

    }
    this->setTextCursor(tc);
}

void QTerminalEdit::appendText(QByteArray newText, bool internal)
{
    QTextCursor tc = this->textCursor();
    QTextCharFormat tcf = tc.charFormat();
    if(internal)
    {
        tcf.setForeground(QBrush(QColor("green")));
        tc.movePosition(QTextCursor::End);

        if(hexMode)
        {
            QString hexFormat;
            while(newText.size()>0)
            {
                QString temp = newText.left(1).toHex().toUpper();
                newText.remove(0,1);
                hexFormat.append(temp);
                hexFormat.append(" ");
                if(temp==QString("0D"))
                {
                    hexFormat.append('\n');
                }
            }

            tc.setCharFormat(tcf);
            tc.insertText(hexFormat);
            tc.movePosition(QTextCursor::End);
        }
        else
        {
            QString data;
            foreach(char ch, newText)
            {
                if(ch !=0)
                {
                    data.append(ch);
                }
            }

            tc.setCharFormat(tcf);
            tc.insertText(data);
            tc.movePosition(QTextCursor::End);

        }
        this->setTextCursor(tc);
    }
    else
    {
        appendText(newText);
    }
}

