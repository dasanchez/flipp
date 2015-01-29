#ifndef PARSERLISTWIDGET_H
#define PARSERLISTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include "../livelistwidget/livelistwidget.h"
#include "../parser/parserwidget.h"

class ParserListWidget : public QWidget
{
    Q_OBJECT
public:
    ParserListWidget(QWidget *parent = 0,QList<ParserUnit*> *pUnits = 0);
    QList<ParserWidget*> *parserList;
    QStringList nameList;
    QList<ParserUnit*> *parsers;
signals:
    void parserListChanged(QStringList);
public slots:

    void newParser();
    void addParser(ParserWidget *);

    void checkAllNames(void);
    void updateList(void);
    QString newParserName(void);

private slots:
    void itemRemoved(int row);
    void sizeChanged(QSize);
//    void resorted(int,int,QListWidgetItem*);
    void parserRemoved(void);

private:
    QLabel *widgetNameLabel;
    QPushButton *newParserButton;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;

    QSplitter *splitter;
    QScrollArea *scrollArea;
    QVBoxLayout *scrollAreaVLayout;
    QWidget *saWidgetContents;

    void setupUI();

private slots:
    void nameChanged(void);
};

#endif // VARIABLELISTWIDGET_H
