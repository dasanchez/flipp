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
signals:
    void plotterConnectionRequest(PlotterWidget*,QString);
    void plotterParserRequest(PlotterWidget*,QString);
    
public slots:
    void newPlotter(void);
    void updateConnections(QStringList*);
    void updateParsers(QStringList*);

private:
    QVBoxLayout *mainLayout;
    QPushButton *newPlotterButton;
    QHBoxLayout *topLayout;
    QLabel *bottomLabel;
    QLabel *widgetNameLabel;

    QSplitter *splitter;
    QScrollArea *scrollArea;
    QVBoxLayout *scrollAreaVLayout;
    QWidget *saWidgetContents;
    QStringList *parserNamesList;
    QStringList *connectionNamesList;

private slots:
    void plotterRequestedConnection(QString);
    void plotterRequestedParser(QString);
    void plotterRemoved(void);
};

#endif // PLOTTERLISTWIDGET_H
