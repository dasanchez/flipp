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
    
public slots:
    void updateConnections(QStringList*);
    void updateParsers(QStringList*);
    void assignConnection(ConnectionWidget *);
    void assignParser(ParserWidget *);
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

    QHBoxLayout *dataSourceLayout;
    QHBoxLayout *contentLayout;
    QVBoxLayout *mainLayout;
    
    ConnectionWidget *connectionWidget;
    ParserWidget *parserWidget;
    ParserEngine *parserEngine;
    QCustomPlot *customPlot;

    QVector<double> keys;
    QList<QVector<double> > valuesList;

    quint8 calcRowCount();
    
private slots:
    void changeConnection(QString);
    void detachConnection(void);
    void changeParser(QString);
    void detachParser(void);
    void populateParserTable();
    void populatePlotArea();
    void parsedDataReady(QList<RepeatedVector> parsedData);
    //     void variableListChanged();
};

#endif // CONNECTIONWIDGET_H
