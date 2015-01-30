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
#include "../livelistwidget/livelistwidget.h"
#include "../parser/vectoritemwidget.h"
#include "../parser/parserengine.h"

class VariableWidget : public QWidget
{
    Q_OBJECT

public:
    VariableWidget(QWidget *parent, ComplexVariable cVar);
    explicit VariableWidget(QWidget *parent = 0);
    ComplexVariable variable;

    int currentType;
    bool matched;
    bool fixed;
    bool hexed;
    bool isExpanded;

    QString getName();
    void setName(QString);
    void setType(int);
    void setFixed(bool);
    void setLength(int);
    void setMatched(bool);
    void setMatchBytes(QByteArray);
    void setRepeat(int);
    QList<VectorItemWidget*> *itemList;

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
    void updateType();
    void updateFixed();
    void toggleLength();
    void changeLength(int);
    void updateMatch();
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
    void addVectorVariable(BaseVariable);
    //    void addVectorByte(QString varName, bool fixed, int length, bool matched, QByteArray mbytes);
    void addVectorNumber(void);
    //    void addVectorNumber(QString varName, bool fixed, int length);
    //    void addVectorNumber(QString varName, bool fixed);

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
    //    QPixmap byteIconPixmap;
    //    QPixmap numberIconPixmap;
    //    QPixmap vectorIconPixmap;
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

    void setupUI();
    void setupUI_fromVariable();
    QString char2hex(QString characters);
    QString hex2char(QString hexChars);
    QRegExp hexRegex;

};

#endif // PARSERWIDGET_H
