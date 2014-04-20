#ifndef WIDGETDRAG_H
#define WIDGETDRAG_H

#include <QWidget>
#include "variablewidget.h"
#include <QList>
#include <QModelIndexList>
#include <QListWidgetItem>
#include <livelistwidget/livelistwidget.h>

class ParserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParserWidget(QWidget *parent = 0);
    ~ParserWidget();
    QList<ComplexVariable> variableList;
    QString getName();
    bool hasValidName(void);
    void setName(QString);
    QList<VariableWidget*> *vwList;
    QString parserName;

signals:
    void updateVariableList();
    void changeSize(QSize);
    void deleteParser(void);
    void nameChange(void);
public slots:
    void setNameValid(bool);
    void newVariable();
    void addVariableWidget(VariableWidget *);
    void remVariable();
private:
    QHBoxLayout *controlLayout;

    QVBoxLayout *mainLayout;

    QLineEdit *nameEdit;
    QLabel *statusBar;
    QPushButton *addByteButton;
    QPushButton *addNumberButton;
    QPushButton *addVectorButton;
    QPushButton *expandButton;
    QPushButton *deleteButton;

    LiveListWidget *lw;

    bool validName;
    bool expanded;

    // Assets
    QPixmap addByteIconPixmap;
    QPixmap addNumberIconPixmap;
    QPixmap addVectorIconPixmap;
    QPixmap moreIconPixmap;
    QPixmap lessIconPixmap;
    QPixmap delIconPixmap;
private slots:
    // Widget handling

    void resorted(int,int,QListWidgetItem*);
    void itemRemoved(int);
    void itemSize(QSize);
    void toggleExpand();

    // Variable data
    //    void nameChanged(QString);
    //    void typeChanged(int);
    //    void lengthToggled(bool);
    //    void lengthChanged(int);
    //    void matchToggled(bool);
    //    void matchChanged(QString);
    //    void repeatChanged(int);

    void variableListChanged();
    // Testing operation
    void printList();
};

#endif // WIDGETDRAG_H
