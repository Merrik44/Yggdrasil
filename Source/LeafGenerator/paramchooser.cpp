#include "paramchooser.h"
#include <QtGui>
#include "sketcharea.h"

ParamChooser::ParamChooser(QWidget *parent) :
    QDialog(parent)
{
    //this->setMaximumHeight(QApplication::desktop()->height());
    createWidgets();
    createConnections();

    QTabWidget* tabs = new QTabWidget(this);
    tabs->setTabsClosable(false);

    QVBoxLayout *colourLayout = new QVBoxLayout;
    QGroupBox* colourBox = new QGroupBox(tr("Leaf Texture Type"));
    QVBoxLayout *colourBoxLayout = new QVBoxLayout;
    QHBoxLayout *leafColourLayout = new QHBoxLayout;
    QHBoxLayout *veinColourLayout = new QHBoxLayout;
    colourBoxLayout->addWidget(colourType);
    colourBox->setLayout(colourBoxLayout);
    colourLayout->addWidget(colourBox);
    leafColourLayout->addWidget(frontColour);
    leafColourLayout->addWidget(backColour);
    colourLayout->addLayout(leafColourLayout);
    veinColourLayout->addWidget(rootColour);
    veinColourLayout->addStretch();
    colourLayout->addLayout(veinColourLayout);
    colourLayout->addStretch();
    QWidget* colourWidget = new QWidget(this);
    colourWidget->setLayout(colourLayout);

    QVBoxLayout *spawnLayout = new QVBoxLayout;
    spawnLayout->addWidget(spawnRate);
    spawnLayout->addWidget(bs);
    spawnLayout->addWidget(bv);
    spawnLayout->addWidget(kd);
    spawnLayout->addStretch();

    QVBoxLayout *growthLayout = new QVBoxLayout;
    QGroupBox* growthBox = new QGroupBox(tr("Growth Type"));
    QVBoxLayout *growthBoxLayout = new QVBoxLayout;
    growthBoxLayout->addWidget(growthType);
    growthBox->setLayout(growthBoxLayout);
    growthLayout->addWidget(growthBox);
    growthLayout->addWidget(hGrowth);
    growthLayout->addWidget(vGrowth);
    growthLayout->addWidget(startScaleVar);
    growthLayout->addWidget(scale);
    growthLayout->addWidget(spec);
    growthLayout->addWidget(bump);
    growthLayout->addStretch();

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(saveButton);
    bottomLayout->addWidget(loadButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(okButton);
    bottomLayout->addWidget(closeButton);
    //bottomLayout->addStretch();

    QHBoxLayout *midLayout = new QHBoxLayout;
    midLayout->addLayout(growthLayout);
    midLayout->addLayout(spawnLayout);
    QWidget* midWidget = new QWidget(this);
    midWidget->setLayout(midLayout);

    tabs->addTab(midWidget,QString("Growth Parameters"));
    tabs->addTab(colourWidget,QString("Colouration Parameters"));
    tabs->widget(1)->setLayout(colourLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabs);
    mainLayout->addLayout(bottomLayout);
   // mainLayout->addStretch();
    setLayout(mainLayout);

    setWindowTitle(tr("Set Parameters"));
    //setFixedHeight(sizeHint().height());
    setFixedHeight(768);
}

void ParamChooser::okClicked()
{
    returnValues(growthType->currentIndex(), hGrowth->getValues(), vGrowth->getValues(), scale->getValues(),
                 colourType->currentIndex(), frontColour->getMainMin(), frontColour->getMainMax(), frontColour->getAltMin(), frontColour->getAltMax(),
                 backColour->getMainMin(), backColour->getMainMax(), backColour->getAltMin(), backColour->getAltMax(),
                 spec->getValues(), bump->getValues(),
                 spawnRate->getValues(), bs->getValues(), bv->getValues(), kd->getValues(),
                 frontColour->getIntensity(), rootColour->getMainMin(), rootColour->getMainMax(), startScaleVar->getValues());
    close();
}

void ParamChooser::createWidgets()
{
    okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    closeButton = new QPushButton(tr("&Cancel"));

    saveButton = new QPushButton(tr("&Save"));
    loadButton = new QPushButton(tr("&Load"));

    growthType = new QComboBox();
    growthType->addItem(tr("Isotropic"));
    growthType->addItem(tr("Anisotropic"));
    growthType->addItem(tr("Marginal"));
    growthType->addItem(tr("None"));

    startScaleVar = new VariationChooser(tr("Starting Growth Step"),0,1,0);

    hGrowth = new VariationChooser(tr("Horizontal Growth Steps"),600,1,0);
    vGrowth = new VariationChooser(tr("Vertical Growth Steps"),600,1,0);
    hGrowth->setMinMax(1,100000);
    vGrowth->setMinMax(1,100000);


    growthTypeChanged(tr("Isotropic"));

    scale = new VariationChooser(tr("Leaf Scale"),1,0.1,2);
    scale->setMinMax(0,1);

    colourType = new QComboBox();
    colourType->addItem(tr("Uniform"));
    colourType->addItem(tr("Colour from veins"));
    colourType->addItem(tr("Colour from edges"));
    colourType->addItem(tr("Speckled"));

    frontColour = new ColourChooser(tr("Front Texture"));
    backColour = new ColourChooser(tr("Back Texture"));
    frontColour->setConstant(true, QString("Constant"));
    backColour->setConstant(true, QString("Constant"));

    spec = new VariationChooser(tr("Leaf Specularity"),0.5,0.1,2);
    spec->setMinMax(0,1);
    bump = new VariationChooser(tr("Leaf Roughness"),0.5,0.1,2);
    bump->setMinMax(0,1);

    spawnRate = new VariationChooser(tr("Auxin Spawn Rate (x10^-6 per Area)"),600,0.1,2);
    bs = new VariationChooser(tr("Auxin Spawn Distance"),2,0.1,2);
    bv = new VariationChooser(tr("Vein Spawn Distance"),2,0.1,2);
    kd = new VariationChooser(tr("Auxin Kill Distance"),10,0.1,2);

    rootColour = new SingleColourChooser(QString("Vein Colour"));

}

void ParamChooser::createConnections()
{
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()),this, SLOT(close()));

    connect(colourType, SIGNAL(currentIndexChanged(QString)), this, SLOT(colourTypeChanged(QString)));
    connect(this, SIGNAL(colourConstant(bool, QString)),frontColour,SLOT(setConstant(bool, QString)));
    connect(this, SIGNAL(colourConstant(bool, QString)),backColour,SLOT(setConstant(bool, QString)));

    connect(growthType, SIGNAL(currentIndexChanged(QString)), this, SLOT(growthTypeChanged(QString)));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadFile()));
}

void ParamChooser::colourTypeChanged(QString typeName)
{
    if(typeName == QString("Uniform"))
    {
        colourConstant(true, typeName);
    }
    else
        colourConstant(false, typeName);
}

void ParamChooser::growthTypeChanged(QString typeName)
{
    //If both x and y have growth rates
    if(typeName == QString("Anisotropic"))
    {
        hGrowth->setVisible(true);
        hGrowth->setName(tr("Horizontal Growth Steps"));
        //Change name
        vGrowth->setVisible(true);
    }
    else
    {
        hGrowth->setVisible(true);
        hGrowth->setName(tr("Total Growth Steps"));
        //Change name
        vGrowth->setVisible(false);

        if(typeName == QString("None"))//Else if neither
        {
            startScaleVar->setVisible(false);
        }
        else
        {
            startScaleVar->setVisible(true);
        }
    }
}

void ParamChooser::saveFile()
{
    QString name = QFileDialog::getSaveFileName(this,
                                                tr("Save As..."), QDir::currentPath(), QString(".lpm"));
    if (!name.isEmpty())
    {
        QFile file(name);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out<<"Leaf Parameter Set"<<"\n\n";

        out<<"Growth Type#"<<growthType->currentIndex()<<"\n";
        out<<"Horizontal Growth#"<<hGrowth->getValues()->x()<<"#"<<hGrowth->getValues()->y()<<"\n";
        out<<"Vertical Growth#"<<vGrowth->getValues()->x()<<"#"<<vGrowth->getValues()->y()<<"\n";
        out<<"Scale#"<<scale->getValues()->x()<<"#"<<scale->getValues()->y()<<"\n";
        out<<"Start Step#"<<startScaleVar->getValues()->x()<<"#"<<startScaleVar->getValues()->y()<<"\n\n";

        out<<"Colour Type#"<<colourType->currentIndex()<<"\n";
        out<<"Front Main Min#"<<frontColour->getMainMin()<<"\n";
        out<<"Front Main Max#"<<frontColour->getMainMax()<<"\n";
        out<<"Front Alt Min#"<<frontColour->getAltMin()<<"\n";
        out<<"Front Alt Max#"<<frontColour->getAltMax()<<"\n";
        out<<"Back Main Min#"<<backColour->getMainMin()<<"\n";
        out<<"Back Main Max#"<<backColour->getMainMax()<<"\n";
        out<<"Back Alt Min#"<<backColour->getAltMin()<<"\n";
        out<<"Back Alt Max#"<<backColour->getAltMax()<<"\n\n";

        out<<"Effect intensity#"<<frontColour->getIntensity()<<"\n";
        out<<"Root Min#"<<rootColour->getMainMin()<<"\n";
        out<<"Root Max#"<<rootColour->getMainMax()<<"\n";
        out<<"Specular#"<<spec->getValues()->x()<<"#"<<spec->getValues()->y()<<"\n";
        out<<"Bump#"<<bump->getValues()->x()<<"#"<<bump->getValues()->y()<<"\n\n";

        out<<"Spawn Rate#"<<spawnRate->getValues()->x()<<"#"<<spawnRate->getValues()->y()<<"\n";
        out<<"Birth from Auxin#"<<bs->getValues()->x()<<"#"<<bs->getValues()->y()<<"\n";
        out<<"Birth from Veins#"<<bv->getValues()->x()<<"#"<<bv->getValues()->y()<<"\n";
        out<<"Auxin kill Distance#"<<kd->getValues()->x()<<"#"<<kd->getValues()->y()<<"\n\n";

        out<<"END";

        file.close();
    }
}

void ParamChooser::loadFile()
{

}
