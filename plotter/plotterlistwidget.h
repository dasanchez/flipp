#ifndef PLOTTERLISTWIDGET_H
#define PLOTTERLISTWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <plotter/plotterwidget.h>

class PlotterListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotterListWidget(QWidget *parent = 0);
    ~PlotterListWidget();
    QList<PlotterWidget*> plotterList;
    QStringList parserNamesList;
    QStringList connectionNamesList;
    QSplitter *splitter;

signals:
    void plotterConnectionRequest(PlotterWidget*,QString);
    void plotterParserRequest(PlotterWidget*,QString);
    
public slots:
    void newPlotter(void);
    void updateConnections(QStringList);
    void updateParsers(QStringList);

    void plotterRequestedConnection(QString);
    void plotterRequestedParser(QString);

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
