#ifndef QTERMINALEDIT_H
#define QTERMINALEDIT_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QDebug>

class QTerminalEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit QTerminalEdit(QWidget *parent = 0);
    void setHex(bool);
    void appendText(QByteArray);
    void appendText(QByteArray, bool);
    bool willEcho(void);
signals:
    void textEntered(QString,bool);
public slots:
    void toggleEcho();
private:
    bool echoBytes;
    bool hexMode;
    void keyPressEvent(QKeyEvent *e);

private slots:

};

#endif // QTERMINALEDIT_H

