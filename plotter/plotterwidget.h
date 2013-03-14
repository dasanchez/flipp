#ifndef PLOTTERWIDGET_H
#define PLOTTERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QList>
#include <QTableWidget>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <connection/connectionwidget.h>
#include <parser/parserwidget.h>
#include <livelistwidget/livelistwidget.h>

class PlotterWidget : public QWidget
{
    Q_OBJECT

public:
    PlotterWidget(QWidget *parent = 0);
    ~PlotterWidget();

public slots:
signals:

private:

    QLabel *widgetNameLabel;
    QPushButton *newProfileButton;
    QComboBox *connectionBox;
    QComboBox *parserBox;
    QTableWidget *tableWidget;

    QHBoxLayout *topLayout;
    QHBoxLayout *dataSourceLayout;
    QVBoxLayout *mainLayout;


private slots:
};

#endif // CONNECTIONWIDGET_H
