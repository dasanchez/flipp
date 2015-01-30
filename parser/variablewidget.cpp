#include "../parser/variablewidget.h"

VariableWidget::VariableWidget(QWidget *parent) :
    QWidget(parent)
{
    currentType = BYTTYPE;
    isExpanded = true;
    fixed=true;
    matched=false;
    hexed=false;

    //    variable = new ComplexVariable;
    variable.name = "variable";
    variable.type = BYTTYPE;
    variable.fixed=true;
    variable.match=false;
    variable.length=1;
    variable.repeat=5;
    variable.matchBytes.clear();

    //variable->vector = new QList<BaseVariable>;
    itemList = new QList<VectorItemWidget*>;

    hexRegex.setPattern(QString("([A-Fa-f0-9]{2}( )?)+"));
    setupUI();
}

VariableWidget::VariableWidget(QWidget *parent, ComplexVariable cVar) :
    QWidget(parent),
    variable(cVar)
{
    currentType = variable.type;
    fixed=variable.fixed;
    matched=variable.match;

    hexed=false;
    isExpanded = true;

    itemList = new QList<VectorItemWidget*>;

    hexRegex.setPattern(QString("([A-Fa-f0-9]{2}( )?)+"));
    setupUI_fromVariable();

    // Populate UI
    nameEdit->setText(variable.name);
    updateType();
    updateFixed();
    updateMatch();
//    update

}

void VariableWidget::nameChanged(QString newName)
{
    variable.name = newName;
    emit variableChanged();
    emit nameChange(newName);
}

QString VariableWidget::getName()
{
    return variable.name;
}

void VariableWidget::setName(QString newName)
{
    disconnect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameChanged(QString)));
    nameEdit->setText(newName);
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameChanged(QString)));

    variable.name = newName;
    emit variableChanged();
    emit nameChange(newName);
}

void VariableWidget::updateType()
{
    switch(variable.type)
    {
    case BYTTYPE:
        typeButton->setText("BYTES");
        mainLayout->removeItem(vectorListLayout);
        repeatLabel->setVisible(false);
        repeatSpin->setVisible(false);
        addByteButton->setVisible(false);
        addNumberButton->setVisible(false);
        moreButton->setVisible(false);
        vectorItemList->setVisible(false);

        lengthButton->setVisible(true);
        lengthButton->setEnabled(true);
        lengthSpin->setVisible(true);
        lengthSpin->setEnabled(true);

        matchButton->setVisible(true);
        matchButton->setEnabled(true);
        matchEdit->setVisible(true);
        matchEdit->setEnabled(true);
        hexButton->setVisible(true);
        hexButton->setEnabled(true);
        currentType=BYTTYPE;
        emit sizeToggled(this->sizeHint());
        break;
    case NUMTYPE:
        typeButton->setText("NUMBER");
        // Widgets
        mainLayout->removeItem(vectorListLayout);

        repeatLabel->setVisible(false);
        repeatSpin->setVisible(false);
        addByteButton->setVisible(false);
        addNumberButton->setVisible(false);
        moreButton->setVisible(false);
        vectorItemList->setVisible(false);

        lengthButton->setVisible(true);
        lengthButton->setEnabled(true);
        lengthSpin->setVisible(true);
        lengthButton->setEnabled(true);

        matchButton->setVisible(true);
        matchButton->setEnabled(false);
        matchEdit->setVisible(true);
        matchEdit->setEnabled(false);
        hexButton->setVisible(true);
        hexButton->setEnabled(false);

        currentType=NUMTYPE;
        emit sizeToggled(this->sizeHint());
    default:
        break;
    }
}

void VariableWidget::setType(int newType)
{
    switch(newType)
    {
    case BYTTYPE:
        setByte();
        break;
    case NUMTYPE:
        setNumber();
        break;
    default:
        setVector();
        break;
    }
    emit variableChanged();
    emit typeChange(variable.type);
}

void VariableWidget::setFixed(bool fixedOn)
{
    QIcon lengthIcon;
    fixed = fixedOn;
    if(fixed)
    {
        lengthIcon=fixlenIconPixmap;
    }
    else
    {
        lengthIcon=varlenIconPixmap;
    }
    lengthButton->setIcon(lengthIcon);
    variable.fixed=fixed;
    emit variableChanged();
    emit lengthToggle(fixed);
}

void VariableWidget::updateFixed()
{
    QIcon lengthIcon;
    if(variable.fixed)
    {
        lengthIcon=fixlenIconPixmap;
        lengthSpin->setValue(variable.length);
    }
    else
    {
        lengthIcon=varlenIconPixmap;
    }
    lengthButton->setIcon(lengthIcon);
}

void VariableWidget::setLength(int newLength)
{
    variable.length=newLength;
    disconnect(lengthSpin,SIGNAL(valueChanged(int)),this,SLOT(changeLength(int)));
    lengthSpin->setValue(newLength);
    connect(lengthSpin,SIGNAL(valueChanged(int)),this,SLOT(changeLength(int)));

    emit variableChanged();
    emit lengthChange(newLength);
}

void VariableWidget::updateMatch()
{
    if(variable.match)
    {
        matchButton->setChecked(true);
        matchEdit->setText(variable.matchBytes);
    }
    else
    {
        matchButton->setChecked(false);
    }
}

void VariableWidget::setMatched(bool matchOn)
{
    QIcon matchIcon;
    matched=matchOn;

    if(matched)
    {
        matchButton->setChecked(true);
    }
    else
    {
        matchButton->setChecked(false);
    }
    variable.match=matched;
    emit variableChanged();
    emit matchToggle(matched);
}

void VariableWidget::setMatchBytes(QByteArray newMatch)
{
    QByteArray characters;
    variable.matchBytes.clear();

    if(hexed)
    {
        // Change all hex data back to ascii representation
        hexRegex.indexIn(newMatch);
        QString temp = hexRegex.cap();

        // Remove all spaces from string
        temp=temp.simplified();
        temp = temp.replace(' ',"");

        // Use a QByteArray to convert
        characters.append(temp);
        characters = QByteArray::fromHex(characters);

    }
    else
    {
        characters.append(newMatch);
    }
    disconnect(matchEdit,SIGNAL(textChanged(QString)),this,SLOT(changeMatch(QString)));
    if(hexed)
        matchEdit->setText(char2hex(characters));
    else
        matchEdit->setText(characters);
    connect(matchEdit,SIGNAL(textChanged(QString)),this,SLOT(changeMatch(QString)));
    variable.matchBytes.append(characters);
    emit variableChanged();
    emit matchChange(characters);
}

void VariableWidget::setRepeat(int newRepeat)
{

    disconnect(repeatSpin,SIGNAL(valueChanged(int)),this,SLOT(changeRepeat(int)));
    repeatSpin->setValue(newRepeat);
    connect(repeatSpin,SIGNAL(valueChanged(int)),this,SLOT(changeRepeat(int)));

    variable.repeat=newRepeat;

    emit variableChanged();
    emit repeatChange(newRepeat);
}

void VariableWidget::toggleType()
{
    switch(currentType)
    {
    case BYTTYPE:
        setNumber();
        break;
    case NUMTYPE:
        setVector();
        break;
    default:
        setByte();
        break;
    }
    emit variableChanged();
    emit typeChange(variable.type);
}

void VariableWidget::setByte()
{
    typeButton->setText("BYTES");

    mainLayout->removeItem(vectorListLayout);

    repeatLabel->setVisible(false);
    repeatSpin->setVisible(false);
    addByteButton->setVisible(false);
    addNumberButton->setVisible(false);
    moreButton->setVisible(false);
    vectorItemList->setVisible(false);

    lengthButton->setVisible(true);
    lengthButton->setEnabled(true);
    lengthSpin->setVisible(true);
    lengthSpin->setEnabled(true);

    matchButton->setVisible(true);
    matchButton->setEnabled(true);
    matchEdit->setVisible(true);
    matchEdit->setEnabled(true);
    hexButton->setVisible(true);
    hexButton->setEnabled(true);

    currentType=BYTTYPE;
    variable.type = BYTTYPE;

    emit sizeToggled(this->sizeHint());
}

void VariableWidget::setNumber()
{
    typeButton->setText("NUMBER");
    // Widgets
    mainLayout->removeItem(vectorListLayout);

    repeatLabel->setVisible(false);
    repeatSpin->setVisible(false);
    addByteButton->setVisible(false);
    addNumberButton->setVisible(false);
    moreButton->setVisible(false);
    vectorItemList->setVisible(false);

    lengthButton->setVisible(true);
    lengthButton->setEnabled(true);
    lengthSpin->setVisible(true);
    lengthButton->setEnabled(true);

    matchButton->setVisible(true);
    matchButton->setEnabled(false);
    matchEdit->setVisible(true);
    matchEdit->setEnabled(false);
    hexButton->setVisible(true);
    hexButton->setEnabled(false);

    currentType=NUMTYPE;
    variable.type = NUMTYPE;
    emit sizeToggled(this->sizeHint());
}

void VariableWidget::setVector()
{
    typeButton->setText("VECTOR");
    moreButton->setIcon(QIcon(moreIconPixmap));
    isExpanded=true;

    lengthButton->setVisible(false);
    lengthSpin->setVisible(false);

    matchButton->setVisible(false);
    matchEdit->setVisible(false);
    hexButton->setVisible(false);

    repeatLabel->setVisible(true);
    repeatSpin->setVisible(true);
    addByteButton->setVisible(true);
    addNumberButton->setVisible(true);
    moreButton->setVisible(true);
    vectorItemList->setVisible(true);
    mainLayout->addLayout(vectorListLayout);

    currentType=VECTYPE;
    variable.type = VECTYPE;
    emit sizeToggled(this->sizeHint());
}

void VariableWidget::toggleLength()
{
    QIcon lengthIcon;
    fixed =!fixed;
    if(fixed)
    {
        lengthIcon=fixlenIconPixmap;
    }
    else
    {
        lengthIcon=varlenIconPixmap;
    }
    lengthButton->setIcon(lengthIcon);
    variable.fixed=fixed;
    emit variableChanged();
    emit lengthToggle(fixed);
}

void VariableWidget::changeLength(int newLength)
{
    variable.length=newLength;
    emit variableChanged();
    emit lengthChange(newLength);
}

void VariableWidget::toggleMatch()
{
    QIcon matchIcon;
    matched=!matched;

    if(matched)
    {
        matchButton->setChecked(true);
    }
    else
    {
        matchButton->setChecked(false);
    }
    variable.match=matched;
    emit variableChanged();
    emit matchToggle(matched);
}

void VariableWidget::toggleHex()
{
    QIcon hexIcon;
    hexed=!hexed;

    if(hexed)
    {
        hexIcon=hexonIconPixmap;
        matchEdit->setText(char2hex(matchEdit->text()));
    }
    else
    {
        hexIcon=hexoffIconPixmap;
        matchEdit->setText(hex2char(matchEdit->text()));
    }
    hexButton->setIcon(hexIcon);
}

void VariableWidget::changeMatch(QString newMatch)
{
    QByteArray characters;
    variable.matchBytes.clear();

    if(hexed)
    {
        // Change all hex data back to ascii representation
        hexRegex.indexIn(newMatch);
        QString temp = hexRegex.cap();

        // Remove all spaces from string
        temp=temp.simplified();
        temp = temp.replace(' ',"");

        // Use a QByteArray to convert
        characters.append(temp);
        characters = QByteArray::fromHex(characters);

    }
    else
    {
        characters.append(newMatch);
    }
    variable.matchBytes.append(characters);
    emit variableChanged();
    emit matchChange(characters);
}

void VariableWidget::changeRepeat(int newRepeat)
{
    variable.repeat=newRepeat;
    emit variableChanged();
    emit repeatChange(newRepeat);
}

void VariableWidget::toggleExpand(bool expand)
{

    if(expand)
    {
        vectorItemList->setVisible(true);
        mainLayout->addLayout(vectorListLayout);
        moreButton->setIcon(QIcon(moreIconPixmap));
    }
    else
    {
        vectorItemList->setVisible(false);
        mainLayout->removeItem(vectorListLayout);
        moreButton->setIcon(QIcon(lessIconPixmap));
    }

    emit sizeToggled(this->sizeHint());
}

void VariableWidget::toggleExpand()
{
    isExpanded = !isExpanded;
    toggleExpand(isExpanded);
}

void VariableWidget::vectorItemNameChanged(QString newName)
{
    VectorItemWidget *iw = static_cast<VectorItemWidget*>(QObject::sender());
    int row = itemList->indexOf(iw);
}

void VariableWidget::vectorItemTypeChanged(int newType)
{
    VectorItemWidget *iw = static_cast<VectorItemWidget*>(QObject::sender());
    int row = itemList->indexOf(iw);
}

void VariableWidget::vectorItemLengthToggled(bool fixed)
{
}

void VariableWidget::vectorItemLengthChanged(int newLength)
{

}

void VariableWidget::vectorItemMatchToggled(bool matched)
{

}

void VariableWidget::vectorItemMatchChanged(QString newMatch)
{
}

void VariableWidget::addVectorByte()
{
    QListWidgetItem *item = new QListWidgetItem;
    VectorItemWidget *iw = new VectorItemWidget(this);
    vectorItemList->addItem(item);
    item->setSizeHint(iw->sizeHint());
    vectorItemList->setItemWidget(item,iw);
    itemList->append(iw);
    BaseVariable bv = iw->variable;
    variable.vector.append(bv);
    connect(iw,SIGNAL(deleteVar()),this,SLOT(vectorItemRemoved()));
    connect(iw,SIGNAL(nameChange(QString)),this,SLOT(vectorItemNameChanged(QString)));
    connect(iw,SIGNAL(variableChanged()),this,SIGNAL(variableChanged()));
    emit variableChanged();
}

void VariableWidget::addVectorVariable(BaseVariable bvar)
{
    QListWidgetItem *item = new QListWidgetItem;
    VectorItemWidget *iw = new VectorItemWidget(this);
    vectorItemList->addItem(item);
    item->setSizeHint(iw->sizeHint());
    vectorItemList->setItemWidget(item,iw);
    iw->variable = bvar;
    iw->setName(bvar.name);
    iw->setType(bvar.type);
    iw->setFixed(bvar.fixed);
    iw->setLength(bvar.length);
    iw->setMatch(bvar.match);
    iw->setMatchBytes(bvar.matchBytes);
    itemList->append(iw);

    variable.vector.append(bvar);
    connect(iw,SIGNAL(deleteVar()),this,SLOT(vectorItemRemoved()));
    connect(iw,SIGNAL(nameChange(QString)),this,SLOT(vectorItemNameChanged(QString)));
    connect(iw,SIGNAL(variableChanged()),this,SIGNAL(variableChanged()));
    emit variableChanged();
}

//void VariableWidget::addVectorByte(QString varName, bool fixed, int length, bool matched, QByteArray mbytes)
//{
//    QListWidgetItem *item = new QListWidgetItem;
//    VectorItemWidget *iw = new VectorItemWidget(this);
//    vectorItemList->addItem(item);
//    item->setSizeHint(iw->sizeHint());
//    vectorItemList->setItemWidget(item,iw);
//    itemList->append(iw);
//    BaseVariable *bv = iw->variable;
//    bv->name = varName;
//    bv->fixed = fixed;
//    bv->length = length;
//    bv->match = matched;
//    bv->matchBytes = mbytes;
//    variable->vector->append(bv);
//}

void VariableWidget::addVectorNumber()
{
    QListWidgetItem *item = new QListWidgetItem;
    VectorItemWidget *iw = new VectorItemWidget(this);
    vectorItemList->addItem(item);
    item->setSizeHint(iw->sizeHint());
    vectorItemList->setItemWidget(item,iw);
    itemList->append(iw);
    variable.vector.append(iw->variable);
    connect(iw,SIGNAL(deleteVar()),this,SLOT(vectorItemRemoved()));
    connect(iw,SIGNAL(nameChange(QString)),this,SLOT(vectorItemNameChanged(QString)));
    connect(iw,SIGNAL(variableChanged()),this,SIGNAL(variableChanged()));
    emit variableChanged();
}

//void VariableWidget::addVectorNumber(QString varName, bool fixed, int length)
//{

//}

void VariableWidget::vectorItemResorted(int src,int dest,QListWidgetItem* item)
{
    // Resort in list:
    itemList->insert(dest,itemList->takeAt(src));
    variable.vector.insert(dest,variable.vector.takeAt(src));
    emit variableChanged();
}

void VariableWidget::vectorItemRemoved()
{
    VectorItemWidget *op_sender = static_cast<VectorItemWidget*>(QObject::sender());
    qDebug() << "signal caught";
    int row;
    for(quint8 i =0;i<vectorItemList->count();i++)
    {
        QListWidgetItem *item = vectorItemList->item(i);
        VectorItemWidget *op_match = static_cast<VectorItemWidget*>(vectorItemList->itemWidget(item));
        if(op_sender==op_match)
        {
            // Row identified
            qDebug() << "Vector item # " << i << "has requested to be removed";
            row=i;

        }
    }
    vectorItemList->removeItemWidget(vectorItemList->item(row));
    vectorItemList->takeItem(row);
    itemList->removeAt(row);
    variable.vector.removeAt(row);
    delete op_sender;
    emit variableChanged();
}

QString VariableWidget::char2hex(QString characters)
{
    QString hexChars;
    // Switch all characters to hex (00-ff) representation
    while(characters.size()>0)
    {
        QByteArray temparray;
        temparray.append(characters.left(1));
        hexChars.append(temparray.toHex().toUpper());
        if(characters.size()>1)
        {
            hexChars.append(" ");
        }
        characters.remove(0,1);
    }
    return hexChars;
}

QString VariableWidget::hex2char(QString hexChars)
{
    // Change all hex data back to ascii representation
    QByteArray characters;
    hexRegex.indexIn(hexChars);
    QString temp = hexRegex.cap();

    // Remove all spaces from string
    temp=temp.simplified();
    temp = temp.replace(' ',"");

    // Use a QByteArray to convert
    characters.append(temp);
    characters = QByteArray::fromHex(characters);

    return characters;
}


void VariableWidget::setupUI()
{
    quint8 controlHeight = 28;
    //    byteIconPixmap = QPixmap(":/images/byte_icon.png");
    //    numberIconPixmap = QPixmap(":/images/number_icon.png");
    //    vectorIconPixmap = QPixmap(":/images/vector_icon.png");
    varlenIconPixmap = QPixmap(":/images/varlen_icon.png");
    fixlenIconPixmap = QPixmap(":/images/fixlen_icon.png");
    matchoffIconPixmap = QPixmap(":/images/matchoff_icon.png");
    matchonIconPixmap = QPixmap(":/images/matchon_icon.png");
    hexoffIconPixmap = QPixmap(":/images/hexoff_icon.png");
    hexonIconPixmap = QPixmap(":/images/hexon_icon.png");
    cycleIconPixmap = QPixmap(":/images/cycle_icon.png");
    addbyteIconPixmap = QPixmap(":/images/addbyte_icon.png");
    addnumberIconPixmap = QPixmap(":/images/addnumber_icon.png");
    moreIconPixmap = QPixmap(":/images/more_icon.png");
    lessIconPixmap = QPixmap(":/images/less_icon.png");
    deleteIconPixmap = QPixmap(":/images/delete_icon.png");
    cycleIconPixmap = cycleIconPixmap.scaled(20,20,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

    //    QIcon typeIcon=byteIconPixmap;
    QIcon lengthIcon=fixlenIconPixmap;
    //    QIcon matchoffIcon = matchoffIconPixmap;
    QIcon hexoffIcon = hexoffIconPixmap;
    QIcon addbyteIcon = addbyteIconPixmap;
    QIcon addnumberIcon = addnumberIconPixmap;
    QIcon moreIcon = moreIconPixmap;
    QIcon deleteIcon = deleteIconPixmap;

    // Title

    // typeButton & lengthButton
    nameEdit = new QLineEdit("variable");
    nameEdit->setToolTip("Enter the variable name");
    nameEdit->setMinimumWidth(40);
    nameEdit->setFixedHeight(controlHeight);
    nameEdit->setFrame(false);
    nameEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    typeButton = new QPushButton("BYTES");
    typeButton->setToolTip("Toggle between byte, number, and vector type");
    typeButton->setFixedWidth(90);
    typeButton->setFixedHeight(controlHeight);

    // Single variable
    lengthButton= new QPushButton;
    lengthButton->setToolTip("Toggle fixed length on or off");
    lengthButton->setIcon(lengthIcon);
    lengthButton->setFixedWidth(24);
    lengthButton->setFixedHeight(controlHeight);

    lengthSpin = new QSpinBox;
    lengthSpin->setToolTip("Enter a fixed length");
    lengthSpin->setValue(1);
    lengthSpin->setMinimum(1);
    lengthSpin->setFixedWidth(40);
    lengthSpin->setFixedHeight(controlHeight);

    matchButton = new QPushButton("Match");
    matchButton->setToolTip("Toggle match on or off");
    matchButton->setFixedWidth(48);
    matchButton->setFixedHeight(controlHeight);
    matchButton->setCheckable(true);
    matchButton->setChecked(false);
    matchButton->setObjectName("matchButton");

    matchEdit = new QLineEdit;
    matchEdit->setToolTip("Enter the byte array to match");
    matchEdit->setFixedHeight(controlHeight);
    matchEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    hexButton = new QPushButton;
    hexButton->setToolTip("Toggle between ASCII and hexadecimal display");
    hexButton->setFixedWidth(24);
    hexButton->setFixedHeight(controlHeight);
    hexButton->setIcon(hexoffIcon);

    // Repeat
    repeatLabel = new QLabel;
    repeatLabel->setPixmap(cycleIconPixmap);
    repeatLabel->setFixedSize(24,controlHeight);
    repeatLabel->setVisible(false);

    repeatSpin = new QSpinBox;
    repeatSpin->setToolTip("Enter the number of times the vector will be received");
    repeatSpin->setMinimum(1);
    repeatSpin->setValue(5);
    repeatSpin->setVisible(false);
    repeatSpin->setFixedWidth(40);
    repeatSpin->setFixedHeight(controlHeight);

    addByteButton = new QPushButton;
    addByteButton->setToolTip("Add byte variable");
    addByteButton->setIcon(addbyteIcon);
    addByteButton->setFixedWidth(30);
    addByteButton->setFixedHeight(controlHeight);
    addByteButton->setVisible(false);

    addNumberButton = new QPushButton;
    addNumberButton->setToolTip("Add number variable");
    addNumberButton->setIcon(addnumberIcon);
    addNumberButton->setFixedWidth(30);
    addNumberButton->setFixedHeight(controlHeight);
    addNumberButton->setVisible(false);

    // Expand / Delete Buttons
    moreButton = new QPushButton;
    moreButton->setToolTip("Show or hide variable details");
    moreButton->setFixedWidth(30);
    moreButton->setFixedHeight(controlHeight);
    moreButton->setIcon(moreIcon);
    moreButton->setVisible(false);

    delButton = new QPushButton;
    delButton->setToolTip("Remove variable");
    delButton->setFixedWidth(30);
    delButton->setFixedHeight(controlHeight);
    delButton->setIcon(deleteIcon);

    titleLayout = new QHBoxLayout;
    titleLayout->addWidget(nameEdit);
    titleLayout->addWidget(typeButton);
    titleLayout->addWidget(lengthButton);
    titleLayout->addWidget(repeatLabel);
    titleLayout->addWidget(lengthSpin);
    titleLayout->addWidget(repeatSpin);
    titleLayout->addWidget(addByteButton);
    titleLayout->addWidget(addNumberButton);
    titleLayout->addWidget(matchButton);
    titleLayout->addWidget(matchEdit);
    titleLayout->addWidget(hexButton);
    titleLayout->addWidget(moreButton);
    titleLayout->addWidget(delButton);
    titleLayout->setMargin(0);

    vectorItemList = new LiveListWidget(this);
    vectorItemList->setVisible(false);
    vectorItemList->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);

    //    addVectorByte();

    vectorListLayout = new QHBoxLayout;
    vectorListLayout->addSpacing(10);
    vectorListLayout->addWidget(vectorItemList);

    // Main
    mainLayout = new QVBoxLayout(this);

    mainLayout->addLayout(titleLayout);
    this->setLayout(mainLayout);
    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);

    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameChanged(QString)));
    connect(delButton,SIGNAL(clicked()),this,SIGNAL(deleteVar()));
    connect(typeButton,SIGNAL(clicked()),this,SLOT(toggleType()));
    connect(lengthButton,SIGNAL(clicked()),this,SLOT(toggleLength()));
    connect(lengthSpin,SIGNAL(valueChanged(int)),this,SLOT(changeLength(int)));
    connect(matchButton,SIGNAL(clicked()),this,SLOT(toggleMatch()));
    connect(hexButton,SIGNAL(clicked()),this,SLOT(toggleHex()));
    connect(matchEdit,SIGNAL(textChanged(QString)),this,SLOT(changeMatch(QString)));
    connect(repeatSpin,SIGNAL(valueChanged(int)),this,SLOT(changeRepeat(int)));
    connect(addByteButton,SIGNAL(clicked()),this,SLOT(addVectorByte()));
    connect(addNumberButton,SIGNAL(clicked()),this,SLOT(addVectorNumber()));
    connect(moreButton,SIGNAL(clicked()),this,SLOT(toggleExpand()));
    connect(vectorItemList,SIGNAL(itemMoved(int,int,QListWidgetItem*)),this,SLOT(vectorItemResorted(int,int,QListWidgetItem*)));

    emit sizeToggled(this->sizeHint());
}

void VariableWidget::setupUI_fromVariable()
{
    quint8 controlHeight = 28;
    //    byteIconPixmap = QPixmap(":/images/byte_icon.png");
    //    numberIconPixmap = QPixmap(":/images/number_icon.png");
    //    vectorIconPixmap = QPixmap(":/images/vector_icon.png");
    varlenIconPixmap = QPixmap(":/images/varlen_icon.png");
    fixlenIconPixmap = QPixmap(":/images/fixlen_icon.png");
    matchoffIconPixmap = QPixmap(":/images/matchoff_icon.png");
    matchonIconPixmap = QPixmap(":/images/matchon_icon.png");
    hexoffIconPixmap = QPixmap(":/images/hexoff_icon.png");
    hexonIconPixmap = QPixmap(":/images/hexon_icon.png");
    cycleIconPixmap = QPixmap(":/images/cycle_icon.png");
    addbyteIconPixmap = QPixmap(":/images/addbyte_icon.png");
    addnumberIconPixmap = QPixmap(":/images/addnumber_icon.png");
    moreIconPixmap = QPixmap(":/images/more_icon.png");
    lessIconPixmap = QPixmap(":/images/less_icon.png");
    deleteIconPixmap = QPixmap(":/images/delete_icon.png");
    cycleIconPixmap = cycleIconPixmap.scaled(20,20,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

    //    QIcon typeIcon=byteIconPixmap;
    QIcon lengthIcon=fixlenIconPixmap;
    //    QIcon matchoffIcon = matchoffIconPixmap;
    QIcon hexoffIcon = hexoffIconPixmap;
    QIcon addbyteIcon = addbyteIconPixmap;
    QIcon addnumberIcon = addnumberIconPixmap;
    QIcon moreIcon = moreIconPixmap;
    QIcon deleteIcon = deleteIconPixmap;

    // Title

    // typeButton & lengthButton
    nameEdit = new QLineEdit;
    nameEdit->setToolTip("Enter the variable name");
    nameEdit->setMinimumWidth(40);
    nameEdit->setFixedHeight(controlHeight);
    nameEdit->setFrame(false);
    nameEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    typeButton = new QPushButton;
    typeButton->setToolTip("Toggle between byte, number, and vector type");
    typeButton->setFixedWidth(90);
    typeButton->setFixedHeight(controlHeight);

    // Single variable
    lengthButton= new QPushButton;
    lengthButton->setToolTip("Toggle fixed length on or off");
    lengthButton->setIcon(lengthIcon);
    lengthButton->setFixedWidth(24);
    lengthButton->setFixedHeight(controlHeight);

    lengthSpin = new QSpinBox;
    lengthSpin->setToolTip("Enter a fixed length");
    lengthSpin->setMinimum(1);
    lengthSpin->setFixedWidth(40);
    lengthSpin->setFixedHeight(controlHeight);

    matchButton = new QPushButton("Match");
    matchButton->setToolTip("Toggle match on or off");
    matchButton->setFixedWidth(48);
    matchButton->setFixedHeight(controlHeight);
    matchButton->setCheckable(true);
    matchButton->setChecked(false);
    matchButton->setObjectName("matchButton");

    matchEdit = new QLineEdit;
    matchEdit->setToolTip("Enter the byte array to match");
    matchEdit->setFixedHeight(controlHeight);
    matchEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    hexButton = new QPushButton;
    hexButton->setToolTip("Toggle between ASCII and hexadecimal display");
    hexButton->setFixedWidth(24);
    hexButton->setFixedHeight(controlHeight);
    hexButton->setIcon(hexoffIcon);

    // Repeat
    repeatLabel = new QLabel;
    repeatLabel->setPixmap(cycleIconPixmap);
    repeatLabel->setFixedSize(24,controlHeight);
    repeatLabel->setVisible(false);

    repeatSpin = new QSpinBox;
    repeatSpin->setToolTip("Enter the number of times the vector will be received");
    repeatSpin->setMinimum(1);
    repeatSpin->setValue(5);
    repeatSpin->setVisible(false);
    repeatSpin->setFixedWidth(40);
    repeatSpin->setFixedHeight(controlHeight);

    addByteButton = new QPushButton;
    addByteButton->setToolTip("Add byte variable");
    addByteButton->setIcon(addbyteIcon);
    addByteButton->setFixedWidth(30);
    addByteButton->setFixedHeight(controlHeight);
    addByteButton->setVisible(false);

    addNumberButton = new QPushButton;
    addNumberButton->setToolTip("Add number variable");
    addNumberButton->setIcon(addnumberIcon);
    addNumberButton->setFixedWidth(30);
    addNumberButton->setFixedHeight(controlHeight);
    addNumberButton->setVisible(false);

    // Expand / Delete Buttons
    moreButton = new QPushButton;
    moreButton->setToolTip("Show or hide variable details");
    moreButton->setFixedWidth(30);
    moreButton->setFixedHeight(controlHeight);
    moreButton->setIcon(moreIcon);
    moreButton->setVisible(false);

    delButton = new QPushButton;
    delButton->setToolTip("Remove variable");
    delButton->setFixedWidth(30);
    delButton->setFixedHeight(controlHeight);
    delButton->setIcon(deleteIcon);

    titleLayout = new QHBoxLayout;
    titleLayout->addWidget(nameEdit);
    titleLayout->addWidget(typeButton);
    titleLayout->addWidget(lengthButton);
    titleLayout->addWidget(repeatLabel);
    titleLayout->addWidget(lengthSpin);
    titleLayout->addWidget(repeatSpin);
    titleLayout->addWidget(addByteButton);
    titleLayout->addWidget(addNumberButton);
    titleLayout->addWidget(matchButton);
    titleLayout->addWidget(matchEdit);
    titleLayout->addWidget(hexButton);
    titleLayout->addWidget(moreButton);
    titleLayout->addWidget(delButton);
    titleLayout->setMargin(0);

    vectorItemList = new LiveListWidget(this);
    vectorItemList->setVisible(false);
    vectorItemList->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);

    vectorListLayout = new QHBoxLayout;
    vectorListLayout->addSpacing(10);
    vectorListLayout->addWidget(vectorItemList);

    // Main
    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(titleLayout);
    this->setLayout(mainLayout);
    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);

    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(nameChanged(QString)));
    connect(delButton,SIGNAL(clicked()),this,SIGNAL(deleteVar()));
    connect(typeButton,SIGNAL(clicked()),this,SLOT(toggleType()));
    connect(lengthButton,SIGNAL(clicked()),this,SLOT(toggleLength()));
    connect(lengthSpin,SIGNAL(valueChanged(int)),this,SLOT(changeLength(int)));
    connect(matchButton,SIGNAL(clicked()),this,SLOT(toggleMatch()));
    connect(hexButton,SIGNAL(clicked()),this,SLOT(toggleHex()));
    connect(matchEdit,SIGNAL(textChanged(QString)),this,SLOT(changeMatch(QString)));
    connect(repeatSpin,SIGNAL(valueChanged(int)),this,SLOT(changeRepeat(int)));
    connect(addByteButton,SIGNAL(clicked()),this,SLOT(addVectorByte()));
    connect(addNumberButton,SIGNAL(clicked()),this,SLOT(addVectorNumber()));
    connect(moreButton,SIGNAL(clicked()),this,SLOT(toggleExpand()));
    connect(vectorItemList,SIGNAL(itemMoved(int,int,QListWidgetItem*)),this,SLOT(vectorItemResorted(int,int,QListWidgetItem*)));

    emit sizeToggled(this->sizeHint());
}
