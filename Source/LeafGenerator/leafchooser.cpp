#include "leafchooser.h"

LeafChooser::LeafChooser(QString name, QWidget *parent) :
    QWidget(parent)
{
    leafName = name;
    createWidgets();

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons->addWidget(isUsed);
    buttons->addWidget(ratio);
    buttons->addStretch();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(picture);
    layout->addLayout(buttons);
    //layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(layout);
}


void LeafChooser::createWidgets()
{
    //Create thumbnail
    pix = QPixmap(leafName+QString("_top.png"));
    pix = pix.scaled(100,100);

    picture = new QLabel();
    picture->setPixmap(pix);
    isUsed = new QCheckBox();
    ratio = new QSpinBox();
    ratio->setPrefix(QString("Ratio: "));
    ratio->setValue(1);
    ratio->setMinimum(1);
    ratio->setEnabled(false);

    connect(isUsed,SIGNAL(toggled(bool)), this, SLOT(leafSelectionClicked(bool)));
}

void LeafChooser::leafSelectionClicked(bool isSelected)
{
    if(isSelected)
    {
        ratio->setEnabled(true);
    }
    else
    {
        ratio->setEnabled(false);
    }
}

bool LeafChooser::isSelected()
{
    return isUsed->isChecked();
}

QString LeafChooser::getName()
{
    return leafName;
}

int LeafChooser::getRatio()
{
    return ratio->value();
}

QSize LeafChooser::sizeHint()
{
    return QSize(120,150);
}

void LeafChooser::enterEvent(QEvent *)
{
    emit leafMousedOver(leafName);
}

void LeafChooser::setSelected(bool selected)
{
    isUsed->setChecked(selected);
}
