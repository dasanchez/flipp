#ifndef TERMINALLISTWIDGET_H
#define TERMINALLISTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QList>

#include "terminalwidget.h"

class TerminalListWidget : public QWidget
{
    Q_OBJECT
public:
    TerminalListWidget(QWidget *parent = 0);
    ~TerminalListWidget();
    QList<TerminalWidget*> terminalList;
    QStringList nameList;
signals:
    void terminalRequest(TerminalWidget*,QString);
public slots:
    void updateConnections(QStringList);
    void newTerminal(void);
    void addTerminal(TerminalWidget *);

    void terminalConnectionRequest(QString);

private:
    QVBoxLayout *mainLayout;
    QPushButton *newTerminalButton;
    QHBoxLayout *topLayout;
    QLabel *bottomLabel;
    QLabel *widgetNameLabel;

    QSplitter *splitter;
    QScrollArea *scrollArea;
    QVBoxLayout *scrollAreaVLayout;
    QWidget *saWidgetContents;


private slots:

    void terminalRemoved(void);

    void sizeChanged(QSize);
};

#endif // TERMINALLISTWIDGET_H
