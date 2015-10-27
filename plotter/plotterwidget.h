#ifndef PLOTTERWIDGET_H
#define PLOTTERWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QList>
#include <QTableWidget>
#include <QComboBox>
#include <QTimer>
#include <QSplitter>
#include <QGroupBox>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QDoubleSpinBox>
#include "../plotter/qcustomplot.h"
#include "../linker/linkerlistwidget.h"
#include "../livelistwidget/livelistwidget.h"
#include "glplotterwidget.h"

class PlotterWidget : public QWidget
{
    Q_OBJECT
    
public:
    PlotterWidget(QList<LinkerUnit*> *linkerList);
    ~PlotterWidget();

public slots:
    void updateLinkerList();
void updateVariableBox();

signals:
//    void removePlotter();

private:
LinkerUnit *linkerUnit;
    QComboBox *linkerBox;
    QComboBox *variableBox;
    QList<LinkerWidget*> linkerList;
    QList<LinkerUnit*> *linkers;

double xval;
double yval;


    QTimer *plotTimer;

    QHBoxLayout *dataSourceLayout;


    QVBoxLayout *mainLayout;
    
//    QCustomPlot *customPlot;
    GLPlotterWidget *glPlot;

    void setupUI();
    
private slots:
    void newData();
    void updatePlot();

};

#endif // CONNECTIONWIDGET_H
