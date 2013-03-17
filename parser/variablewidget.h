#ifndef VARIABLEWIDGET_H
#define VARIABLEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QDebug>
#include <livelistwidget/livelistwidget.h>
#include <parser/vectoritemwidget.h>
#include <parser/parserengine.h>

class VariableWidget : public QWidget
{
    Q_OBJECT

private:
    QVBoxLayout *mainLayout;

    // Title
    QLineEdit *nameEdit;
    QPushButton *typeButton;
    QPushButton *lengthButton;
    QPushButton *matchButton;
    QPushButton *moreButton;
    QPushButton *delButton;
    QHBoxLayout *titleLayout;

    QSpinBox *lengthSpin;
    QLineEdit *matchEdit;
    QPushButton *hexButton;

    // Vector variable
    QLabel *repeatLabel;
    QSpinBox *repeatSpin;

    QPushButton *addByteButton;
    QPushButton *addNumberButton;

    LiveListWidget *vectorItemList;
    QHBoxLayout *vectorListLayout;

    // Assets
    QPixmap byteIconPixmap;
    QPixmap numberIconPixmap;
    QPixmap vectorIconPixmap;
    QPixmap varlenIconPixmap;
    QPixmap fixlenIconPixmap;
    QPixmap matchoffIconPixmap;
    QPixmap matchonIconPixmap;
    QPixmap hexonIconPixmap;
    QPixmap hexoffIconPixmap;
    QPixmap cycleIconPixmap;
    QPixmap addbyteIconPixmap;
    QPixmap addnumberIconPixmap;
    QPixmap moreIconPixmap;
    QPixmap lessIconPixmap;
    QPixmap deleteIconPixmap;

    int currentType;
    bool matched;
    bool fixed;
    bool hexed;
    bool isExpanded;

    QList<VectorItemWidget*> *itemList;

    void setupUI();
    QString char2hex(QString characters);
    QString hex2char(QString hexChars);
    QRegExp hexRegex;

public:
    explicit VariableWidget(QWidget *parent = 0);
    ComplexVariable *variable;

signals:
    void nameChange(QString);
    void typeChange(int);
    void lengthToggle(bool);
    void lengthChange(int);
    void matchToggle(bool);
    void matchChange(QByteArray);
    void repeatChange(int);
    void vectorChanged();
    void variableChanged();

    void sizeToggled(QSize);
    void deleteVar();
public slots:
    void toggleExpand(bool);
    void toggleExpand();
    void nameChanged(QString);
    void setByte();
    void setNumber();
    void setVector();
    void toggleType();
    void toggleLength();
    void changeLength(int);
    void toggleMatch();
    void changeMatch(QString);
    void toggleHex();
    void changeRepeat(int);
    void vectorItemNameChanged(QString);
    void vectorItemTypeChanged(int);
    void vectorItemLengthToggled(bool);
    void vectorItemLengthChanged(int);
    void vectorItemMatchToggled(bool);
    void vectorItemMatchChanged(QString);
    void vectorItemResorted(int,int,QListWidgetItem*);
    void vectorItemRemoved();
    void addVectorByte(void);
    void addVectorNumber(void);
};

#endif // PARSERWIDGET_H
