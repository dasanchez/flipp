#ifndef DATASTAGINGWIDGET_H
#define DATASTAGINGWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class DataStagingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataStagingWidget(QWidget *parent = 0);
    QLabel *connectionLabel;
    QComboBox *connectionBox;
    QLabel *parserLabel;
    QComboBox *parserBox;

    QHBoxLayout *linkLayout;
signals:
    
public slots:
    
};

#endif // DATASTAGINGWIDGET_H
