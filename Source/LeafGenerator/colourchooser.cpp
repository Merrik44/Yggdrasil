#include "colourchooser.h"

ColourChooser::ColourChooser(QString title, QWidget *parent) :
    QWidget(parent)
{
    isConstant = false;

    mainMinBtn = new QPushButton(tr("From"));
    mainMin = new QColor(0,64,0);
    mainMinBtn->setIcon(createIcon(mainMin));
    mainMinBtn->setObjectName(QString("mainMinBtn"));

    mainMaxBtn = new QPushButton(tr("To"));
    mainMax = new QColor(0,128,0);
    mainMaxBtn->setIcon(createIcon(mainMax));
    mainMaxBtn->setObjectName(QString("mainMaxBtn"));

    altMinBtn = new QPushButton(tr("From"));
    altMin = new QColor(32,64,32);
    altMinBtn->setIcon(createIcon(altMin));
    altMinBtn->setObjectName(QString("altMinBtn"));

    altMaxBtn = new QPushButton(tr("To"));
    altMax = new QColor(32,128,32);
    altMaxBtn->setIcon(createIcon(altMax));
    altMaxBtn->setObjectName(QString("altMaxBtn"));

    intensity = new VariationChooser(QString("Colouring Spread Distance"), 2,1,0);
    intensity->setMinimumHeight(50);

    connect(mainMinBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(mainMaxBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(altMinBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(altMaxBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));

    sideImage = new QLabel();
    sideImage->setPixmap(createVPixmap());
    mainImage = new QLabel();
    mainImage->setPixmap(createHPixmap(mainMin,mainMax));
    altImage = new QLabel();
    altImage->setPixmap(createHPixmap(altMin,altMax));

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

    //Alt Colour Box
    bottomBox = new QGroupBox(tr("Alternate Colour:"));
    QVBoxLayout *bottomBoxLayout = new QVBoxLayout();
    bottomBoxLayout->addWidget(new QLabel(tr("Variation")),0,Qt::AlignCenter);

    QHBoxLayout *bottomBoxHLayout = new QHBoxLayout();
    bottomBoxHLayout->addWidget(altMinBtn);
    bottomBoxHLayout->addStretch();
    bottomBoxHLayout->addWidget(altMaxBtn);
    bottomBoxLayout->addLayout(bottomBoxHLayout);

    bottomBoxLayout->addWidget(new QLabel(tr("Range")),0,Qt::AlignCenter);
    bottomBoxLayout->addWidget(altImage,0,Qt::AlignCenter);
    bottomBoxLayout->addWidget(intensity,0,Qt::AlignCenter);
    bottomBoxLayout->addStretch();

    bottomBox->setLayout(bottomBoxLayout);

    //Now add to left layout
    leftLayout->addWidget(topBox);
    leftLayout->addWidget(bottomBox);


    QHBoxLayout *boxLayout = new QHBoxLayout();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    boxLayout->addLayout(leftLayout);
    boxLayout->addWidget(sideImage,0,Qt::AlignBottom);
    box->setLayout(boxLayout);
    mainLayout->addWidget(box);

    this->setLayout(mainLayout);
}

QPixmap ColourChooser::createHPixmap(QColor* min, QColor* max)
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

QPixmap ColourChooser::createVPixmap()
{
    int height = 250;

    QImage im = QImage(20,height, QImage::Format_RGB32);
    int h,s,v;

    //Take the average
    h = (int)((float)(mainMax->hue()+mainMin->hue())/(float)2);
    s = (int)((float)(mainMax->saturation()+mainMin->saturation())/(float)2);
    v = (int)((float)(mainMax->value()+mainMin->value())/(float)2);
    QColor main = main.fromHsv(h,s,v);

    h = (int)((float)(altMax->hue()+altMin->hue())/(float)2);
    s = (int)((float)(altMax->saturation()+altMin->saturation())/(float)2);
    v = (int)((float)(altMax->value()+altMin->value())/(float)2);
    QColor alt = alt.fromHsv(h,s,v);

    QColor* c = new QColor(main);
    for(int i=0; i<height;i++)
    {
        if(!isConstant)
        {
            h = (int)(alt.hue()*(float)i/(float)height + main.hue()*(float)(height-i)/(float)height);
            s = (int)(alt.saturation()*(float)i/(float)height + main.saturation()*(float)(height-i)/(float)height);
            v = (int)(alt.value()*(float)i/(float)height + main.value()*(float)(height-i)/(float)height);
            *c = c->fromHsv(h,s,v);
        }

        for(int j=0; j< 20; j++)
            im.setPixel(j,i,c->rgb());
    }
    return QPixmap().fromImage(im);
}

QIcon ColourChooser::createIcon(QColor* c)
{
    QImage im = QImage(20,20, QImage::Format_RGB32);
    im.fill(c->rgb());
    QIcon i = QIcon(QPixmap().fromImage(im));
    return i;
}

void ColourChooser::btnClicked()
{
    QString name = sender()->objectName();

    if(name == QString("mainMaxBtn"))
    {
        QColor c = QColorDialog().getColor(mainMax->toRgb(),0,tr("Set Main Colour Maximum Variation"));
        if(c != QColor::Invalid)
        {
            *mainMax = c;
            mainMaxBtn->setIcon(createIcon(mainMax));
            mainImage->setPixmap(createHPixmap(mainMin,mainMax));
        }
    }
    else if(name == QString("mainMinBtn"))
    {
        QColor c = QColorDialog().getColor(mainMin->toRgb(),0,tr("Set Main Colour Minimum Variation"));
        if(c != QColor::Invalid)
        {
            *mainMin = c;
            mainMinBtn->setIcon(createIcon(mainMin));
            mainImage->setPixmap(createHPixmap(mainMin,mainMax));
        }
    }
    else if(name == QString("altMaxBtn"))
    {
        QColor c = QColorDialog().getColor(altMax->toRgb(),0,tr("Set ")+ bottomBox->title()+QString(" Maximum Variation"));
        if(c != QColor::Invalid)
        {
            *altMax = c;
            altMaxBtn->setIcon(createIcon(altMax));
            altImage->setPixmap(createHPixmap(altMin,altMax));
        }
    }
    else if(name == QString("altMinBtn"))
    {
        QColor c = QColorDialog().getColor(altMin->toRgb(),0,tr("Set ")+ bottomBox->title()+QString(" Minimum Variation"));
        if(c != QColor::Invalid)
        {
            *altMin = c;
            altMinBtn->setIcon(createIcon(altMin));
            altImage->setPixmap(createHPixmap(altMin,altMax));
        }
    }

    sideImage->setPixmap(createVPixmap());

}

QColor* ColourChooser::getMainMin()
{
    return mainMin;
}

QColor* ColourChooser::getMainMax()
{
    return mainMax;
}

QColor* ColourChooser::getAltMin()
{
    return altMin;
}

QColor* ColourChooser::getAltMax()
{
    return altMax;
}

QPointF* ColourChooser::getIntensity()
{
    return intensity->getValues();
}

void ColourChooser::setConstant(bool value, QString type)
{
    isConstant = value;

    if(isConstant)
    {
        bottomBox->setEnabled(false);
        sideImage->setPixmap(createVPixmap());   
    }
    else
    {
        bottomBox->setEnabled(true);
        sideImage->setPixmap(createVPixmap());
        bottomBox->setTitle(type+QString(" colour"));
    }
}
