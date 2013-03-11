#ifndef PARSERLISTWIDGET_H
#define PARSERLISTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <livelistwidget/livelistwidget.h>

class ParserListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParserListWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    
private:
    QLabel *widgetNameLabel;
    QPushButton *newParserButton;
    LiveListWidget *listWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
};

#endif // VARIABLELISTWIDGET_H
