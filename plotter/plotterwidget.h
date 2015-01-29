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

class PlotterWidget : public QWidget
{
    Q_OBJECT
    
public:
    PlotterWidget(QWidget *parent, QList<LinkerUnit*> *linkerList);
    PlotterWidget(QWidget *parent = 0);
    ~PlotterWidget();

public slots:
    void updateLinkerList(QList<LinkerWidget*>);
void updateVariableBox();
    //    void setXRange(double);
//    void setYMin(QString);
//    void setYMax(QString);

signals:
//    void removePlotter();

private:
    QComboBox *linkerBox;
    QComboBox *variableBox;
    QList<LinkerWidget*> linkerList;
    QList<LinkerUnit*> *linkers;
//    QPushButton *toggleTableButton;
//    QPushButton *removeButton;
//    QPixmap delIconPixmap;
//    QTableWidget *tableWidget;

    // Plot settings
//    QLabel *xRangeLabel;
//    QDoubleSpinBox *xRangeSpin;
//    QHBoxLayout *xRangeLayout;
//    QLabel *xTicksLabel;
//    QPushButton *xTicksButton;
//    QSpinBox *xTicksSpin;
//    QHBoxLayout *xTicksLayout;

//    QVBoxLayout *xAxisLayout;
//    QGroupBox *xAxisSettingsBox;


//    QLabel *yMinLabel;
//    QLabel *yMaxLabel;
//    QSpinBox *yMinSpin;
//    QSpinBox *yMaxSpin;

    QTimer *plotTimer;

//    QHBoxLayout *tableLayout;

    QHBoxLayout *dataSourceLayout;
//    QVBoxLayout *plotLayout;
//    QSplitter *contentSplitter;

//    QPushButton *showPlotSettingsButton;


//    QGroupBox *yAxisSettingsBox;
//    QVBoxLayout *plotSettingsLayout;
//    QPushButton *yAxisRangeButton;
//    QHBoxLayout *yAxisMinLayout;
//    QHBoxLayout *yAxisMaxLayout;
//    QVBoxLayout *yAxisLayout;

    QVBoxLayout *mainLayout;
    
    QCustomPlot *customPlot;

//    double xMax;
//    double yMin;
//    double yMax;
//    QVector<double> keys;
//    QList<QVector<double> > valuesList;

//    quint8 calcRowCount();
//    void adjustXRange();
//    bool yAxisAutoRange;

    void setupUI();
    
private slots:
//    void populatePlotArea();
    void newData();
    void updatePlot();
//    void togglexAxisAutoTicks(bool);
//    void xTicksChanged(int);
//    void toggleyAxisAutoRange(bool);
//    void toggleSettingsVisible(bool);
};

#endif // CONNECTIONWIDGET_H
