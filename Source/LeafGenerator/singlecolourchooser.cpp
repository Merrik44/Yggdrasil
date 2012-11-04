#include "singlecolourchooser.h"

SingleColourChooser::SingleColourChooser(QString title, QWidget *parent) :
    QWidget(parent)
{
    isConstant = false;

    mainMinBtn = new QPushButton(tr("From"));
    mainMin = new QColor(133,232,117);
    mainMinBtn->setIcon(createIcon(mainMin));
    mainMinBtn->setObjectName(QString("mainMinBtn"));

    mainMaxBtn = new QPushButton(tr("To"));
    mainMax = new QColor(217,255,128);
    mainMaxBtn->setIcon(createIcon(mainMax));
    mainMaxBtn->setObjectName(QString("mainMaxBtn"));


    connect(mainMinBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(mainMaxBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));


    mainImage = new QLabel();
    mainImage->setPixmap(createHPixmap(mainMin,mainMax));


    QGroupBox *box = new QGroupBox(title);
    QVBoxLayout *leftLayout = new QVBoxLayout();

    //Main Colour Box
    QGroupBox *topBox = new QGroupBox(tr("Main Colour:"));
    QVBoxLayout *topBoxLayout = new QVBoxLayout();
    topBoxLayout->addWidget(new QLabel(tr("Variation")),0,Qt::AlignCenter);

    QHBoxLayout *topBoxHLayout = new QHBoxLayout();
    topBoxHLayout->addWidget(mainMinBtn);
    topBoxHLayout->addStretch();
    topBoxHLayout->addWidget(mainMaxBtn);
    topBoxLayout->addLayout(topBoxHLayout);

    topBoxLayout->addWidget(new QLabel(tr("Range")),0,Qt::AlignCenter);
    topBoxLayout->addWidget(mainImage,0,Qt::AlignCenter);

    topBox->setLayout(topBoxLayout);


    //Now add to left layout
    leftLayout->addWidget(topBox);


    QHBoxLayout *boxLayout = new QHBoxLayout();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    boxLayout->addLayout(leftLayout);
    box->setLayout(boxLayout);
    mainLayout->addWidget(box);

    this->setLayout(mainLayout);
}

QPixmap SingleColourChooser::createHPixmap(QColor* min, QColor* max)
{
    QImage im = QImage(150,20, QImage::Format_RGB32);
    QColor* c = new QColor(*min);
    int h,s,v;

    for(int i=0; i<150;i++)
    {
        h = (int)(max->hue()*(float)i/(float)150 + min->hue()*(float)(150-i)/(float)150);
        s = (int)(max->saturation()*(float)i/(float)150 + min->saturation()*(float)(150-i)/(float)150);
        v = (int)(max->value()*(float)i/(float)150 + min->value()*(float)(150-i)/(float)150);

        *c = c->fromHsv(h,s,v);
        for(int j=0; j< 20; j++)
            im.setPixel(i,j,c->rgb());
    }
    return QPixmap().fromImage(im);
}

QIcon SingleColourChooser::createIcon(QColor* c)
{
    QImage im = QImage(20,20, QImage::Format_RGB32);
    im.fill(c->rgb());
    QIcon i = QIcon(QPixmap().fromImage(im));
    return i;
}

void SingleColourChooser::btnClicked()
{
    QString name = sender()->objectName();

    if(name == QString("mainMaxBtn"))
    {
        *mainMax = QColorDialog().getColor(mainMax->toRgb(),0,tr("Set Main Colour Maximum Variation"));
        mainMaxBtn->setIcon(createIcon(mainMax));
        mainImage->setPixmap(createHPixmap(mainMin,mainMax));
    }
    else if(name == QString("mainMinBtn"))
    {
        *mainMin = QColorDialog().getColor(mainMin->toRgb(),0,tr("Set Main Colour Minimum Variation"));
        mainMinBtn->setIcon(createIcon(mainMin));
        mainImage->setPixmap(createHPixmap(mainMin,mainMax));
    }
}

QColor* SingleColourChooser::getMainMin()
{
    return mainMin;
}

QColor* SingleColourChooser::getMainMax()
{
    return mainMax;
}
