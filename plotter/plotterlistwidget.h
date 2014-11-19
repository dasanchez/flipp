#ifndef PLOTTERLISTWIDGET_H
#define PLOTTERLISTWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include "../plotter/plotterwidget.h"

class PlotterListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotterListWidget(QWidget *parent = 0);
    ~PlotterListWidget();
    QList<PlotterWidget*> plotterList;
    QSplitter *splitter;

signals:

    
public slots:
    void newPlotter(void);


private:
    QVBoxLayout *mainLayout;
    QPushButton *newPlotterButton;
    QHBoxLayout *topLayout;
    QLabel *bottomLabel;
    QLabel *widgetNameLabel;


    QScrollArea *scrollArea;
    QVBoxLayout *scrollAreaVLayout;
    QWidget *saWidgetContents;


private slots:

    void plotterRemoved(void);
};

#endif // PLOTTERLISTWIDGET_H
