#ifndef DATASTAGINGWIDGET_H
#define DATASTAGINGWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QComboBox>

#include "parser/parserengine.h"

class DataStagingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataStagingWidget(QWidget *parent = 0);
    QStringList connectionNamesList;
    QStringList parserNamesList;

    ~DataStagingWidget();
signals:
    //void stagingConnectionRequest(PlotterWidget*,QString);
    //void staginParserRequest(PlotterWidget*,QString);
public slots:

    void updateConnections(QStringList);
    void updateParsers(QStringList);

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    QPushButton *newVariableButton;
    QLabel *bottomLabel;
    QLabel *widgetNameLabel;

    QComboBox *connectionBox;
    QComboBox *parserBox;

    QHBoxLayout *dataSourceLayout;

//    QScrollArea *scrollArea;
//    QVBoxLayout *scrollAreaVLayout;
//    QWidget *saWidgetContents;
ParserEngine *parserEngine;
};

#endif // DATASTAGINGWIDGET_H
