#ifndef PARSERLISTWIDGET_H
#define PARSERLISTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <livelistwidget/livelistwidget.h>
#include <parser/parserwidget.h>

class ParserListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParserListWidget(QWidget *parent = 0);
    QList<ParserWidget*> *parserList;
signals:

public slots:

    void newParser();

private slots:
    void itemRemoved(int row);
    void sizeChanged(QSize);
    void resorted(int,int,QListWidgetItem*);
    void parserRemoved(void);

private:
    QLabel *widgetNameLabel;
    QPushButton *newParserButton;
    LiveListWidget *listWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;

};

#endif // VARIABLELISTWIDGET_H
