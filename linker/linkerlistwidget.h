#ifndef LINKERLISTWIDGET_H
#define LINKERLISTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <livelistwidget/livelistwidget.h>
#include <linker/linkerwidget.h>

class LinkerListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LinkerListWidget(QWidget *parent = 0);
    QList<LinkerWidget*> *linkerList;
    QStringList nameList;

signals:

public slots:

    void newLinker();
    void addLinker(LinkerWidget *);

private:
    QLabel *widgetNameLabel;
    QPushButton *newLinkerButton;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;

    QSplitter *splitter;
    QScrollArea *scrollArea;
    QVBoxLayout *scrollAreaVLayout;
    QWidget *saWidgetContents;
};

#endif // LINKERLISTWIDGET_H
