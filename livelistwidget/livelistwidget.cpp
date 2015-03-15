#include "livelistwidget.h"

LiveListWidget::LiveListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
//    setAutoFillBackground(true);

}

void LiveListWidget::dropEvent(QDropEvent *event)
{
    QListWidget::dropEvent(event);
    emit itemMoved(drag_row,this->row(temp_item),temp_item);
}

void LiveListWidget::startDrag(Qt::DropActions supportedActions)
{
    temp_item  = currentItem();
    drag_row= this->row(temp_item);
    QListWidget::startDrag(supportedActions);
}

void LiveListWidget::keyPressEvent(QKeyEvent *event)
{
    temp_item = currentItem();
    if(event->key()==Qt::Key_Delete)
    {
        emit itemRemoved(this->row(temp_item));
        this->takeItem(this->row(temp_item));
        delete temp_item;
    }

}
