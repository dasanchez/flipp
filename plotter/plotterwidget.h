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
#include <QStyledItemDelegate>
#include <connection/connectionwidget.h>
#include <parser/parserwidget.h>
#include <parser/parserengine.h>
#include <plotter/qcustomplot.h>
#include <livelistwidget/livelistwidget.h>

class PlotterWidget : public QWidget
{
    Q_OBJECT
    
public:
    PlotterWidget(QWidget *parent = 0);
    ~PlotterWidget();
    QString currentConnection();
    QString currentParser();

public slots:
    void updateConnections(QStringList);
    void updateParsers(QStringList);
    void assignConnection(ConnectionWidget *);
    void assignParser(ParserWidget *);
    void changeConnection(QString);
    void changeParser(QString);
    void setXRange(double);
    void setYMin(QString);
    void setYMax(QString);

signals:
    void plotterConnectionRequest(QString);
    void plotterParserRequest(QString);
    void removePlotter();
private:
    QLabel *connectionLabel;
    QComboBox *connectionBox;
    QLabel *parserLabel;
    QComboBox *parserBox;
    QPushButton *toggleTableButton;
    QPushButton *removeButton;
    QPixmap delIconPixmap;
    QTableWidget *tableWidget;

    // Plot settings
    QLabel *xRangeLabel;
    QDoubleSpinBox *xRangeSpin;
    QHBoxLayout *xRangeLayout;
    QLabel *xTicksLabel;
    QPushButton *xTicksButton;
    QSpinBox *xTicksSpin;
    QHBoxLayout *xTicksLayout;

    QVBoxLayout *xAxisLayout;
    QGroupBox *xAxisSettingsBox;


    QLabel *yMinLabel;
    QLabel *yMaxLabel;
    QSpinBox *yMinSpin;
    QSpinBox *yMaxSpin;

    QTimer *plotTimer;

    QHBoxLayout *tableLayout;

    QHBoxLayout *dataSourceLayout;
    QVBoxLayout *plotLayout;
    QSplitter *contentSplitter;

    QPushButton *showPlotSettingsButton;


    QGroupBox *yAxisSettingsBox;
    QVBoxLayout *plotSettingsLayout;
    QPushButton *yAxisRangeButton;
    QHBoxLayout *yAxisMinLayout;
    QHBoxLayout *yAxisMaxLayout;
    QVBoxLayout *yAxisLayout;

    QVBoxLayout *mainLayout;
    
    ConnectionWidget *connectionWidget;
    ParserWidget *parserWidget;
    ParserEngine *parserEngine;
    QCustomPlot *customPlot;

    double xMax;
    double yMin;
    double yMax;
    QVector<double> keys;
    QList<QVector<double> > valuesList;

    quint8 calcRowCount();
    void adjustXRange();
    bool yAxisAutoRange;

    void setupUI();
    
private slots:
    void detachConnection(void);
    void detachParser(void);
    void populateParserTable();
    void populatePlotArea();
    void parsedDataReady(QList<RepeatedVector> parsedData);
    void updatePlot();
    void togglexAxisAutoTicks(bool);
    void xTicksChanged(int);
    void toggleyAxisAutoRange(bool);
    void toggleSettingsVisible(bool);
};

#endif // CONNECTIONWIDGET_H
