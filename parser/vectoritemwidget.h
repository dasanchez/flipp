#ifndef VECTORITEMWIDGET_H
#define VECTORITEMWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QHeaderView>
#include <QDebug>

#include <parser/parserengine.h>

#define BYTTYPE 0
#define NUMTYPE 1

class VectorItemWidget : public QWidget
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
    QPixmap deleteIconPixmap;


    QRegExp hexRegex;

    QString char2hex(QString characters);
    QString hex2char(QString hexChars);
    void setupUi();

public:
    explicit VectorItemWidget(QWidget *parent = 0);
    BaseVariable variable;
    int currentType;
    bool matched;
    bool fixed;
    bool hexed;
    void setName(QString);
    void setType(int);
    void setFixed(bool);
    void setLength(int);
    void setMatch(bool);
    void setMatchBytes(QByteArray);


signals:
    void nameChange(QString);
    void typeChange(int);
    void lengthToggle(bool);
    void lengthChange(int);
    void matchToggle(bool);
    void matchChange(QByteArray);
    void sizeToggled(QSize);
    void deleteVar();
    void variableChanged();
public slots:
    void setByte();
    void setNumber();
    void changeName(QString);
    void toggleType();
    void toggleLength();
    void changeLength(int);
    void toggleMatch();
    void changeMatch(QString);
    void toggleHex();
};


#endif // VECTORITEMWIDGET_H
