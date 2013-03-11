#ifndef LIVELISTWIDGET_H
#define LIVELISTWIDGET_H

#include <QListWidget>
#include <QKeyEvent>

class LiveListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit LiveListWidget(QWidget *parent = 0);
    
signals:
    void itemMoved(int,int,QListWidgetItem*);
    void itemRemoved(int);
public slots:

private:
    QListWidgetItem *temp_item;
    int drag_row;

protected:
   void dropEvent(QDropEvent *event);
   void startDrag(Qt::DropActions supportedActions);
   void keyPressEvent(QKeyEvent *event);

};

#endif // LIVELISTWIDGET_H
