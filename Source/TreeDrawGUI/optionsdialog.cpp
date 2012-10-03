#include "optionsdialog.h"
#include "iostream"
#include "TextureSynthesis/texturesynthesisdialog.h"
const int TEX_DISPLAY_SIZE = 96;
const int TEX_PER_ROW = 3;

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent)
{

    //QFrame *frame = new QFrame;
    layout = new QGridLayout(this);
    frameLayout = new QGridLayout();
    layout->addWidget(&settingsHeading, 0, 0);
    layout->addLayout(frameLayout, 1, 0);
    layout->addWidget(&textureHeading, 0, 1);
    
    layout->addWidget(&texArea, 1, 1);
    
    layout->addWidget(&generateTexture,2,1);
    

    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(&ok, 3, 0);
    layout->setAlignment(&ok, Qt::AlignRight);
    layout->addWidget(&cancel, 3, 1);

    layout->setSpacing(20);
    //frameLayout->setMargin(1);

    generateTexture.setText("Generate new texture");
    textureDialog = new TextureSynthesisDialog();
    connect(&generateTexture, SIGNAL(triggered()), this, SLOT(showTextureDialog()));
    //generateTexture.setMaximumWidth(80);
    
    ok.setText("Ok");
    ok.setMaximumWidth(80);
    cancel.setText("Cancel");
    cancel.setMaximumWidth(80);

    layout->setAlignment(&textureHeading, Qt::AlignCenter);
    textureHeading.setText("Select the tree texture");
    layout->setAlignment(&settingsHeading, Qt::AlignCenter);
    settingsHeading.setText("Select the generation settings");



    label1.setText("Additional levels of detail");
    value1.setAlignment(Qt::AlignCenter);
    enabled1.setChecked(true);
    enabled1.setText("");
    option1.setOrientation(Qt::Horizontal);
    option1.setEnabled(true);
    option1.setRange(1,10);
    option1.setMinimumWidth(200);
    option1.setMaximumWidth(200);

    label2.setText("Branch limit");
    value2.setAlignment(Qt::AlignCenter);
    enabled2.setChecked(true);
    enabled2.setText("");
    option2.setOrientation(Qt::Horizontal);
    option2.setEnabled(true);
    option2.setRange(50 ,4000);
    option2.setMinimumWidth(200);
    option2.setMaximumWidth(200);

    label3.setText("Propagation limit");
    value3.setAlignment(Qt::AlignCenter);
    enabled3.setChecked(false);
    enabled3.setText("");
    option3.setOrientation(Qt::Horizontal);
    option3.setEnabled(false);
    option3.setRange(1,5);
    option3.setMinimumWidth(200);
    option3.setMaximumWidth(200);


    subDivLabel.setText("Subdivisions applied to mesh");
    subDivValue.setAlignment(Qt::AlignCenter);
    subDivSlider.setOrientation(Qt::Horizontal);
    subDivSlider.setEnabled(true);
    subDivSlider.setRange(0 ,4);
    subDivSlider.setMinimumWidth(200);
    subDivSlider.setMaximumWidth(200);

    label4.setText("Replicate trunk exactly as drawn");
    enabled3.setChecked(true);
    enabled3.setText("");
    value4.setText("");

    frameLayout->addWidget(&label1, 0, 1);
    frameLayout->addWidget(&enabled1, 1, 0);
    frameLayout->addWidget(&option1, 1, 1);
    frameLayout->addWidget(&value1, 1, 2);

    frameLayout->addWidget(&label2, 2, 1);
    //frameLayout->addWidget(&enabled2, 3, 0);
    frameLayout->addWidget(&option2, 3, 1);



    frameLayout->addWidget(&value2, 3, 2);



    frameLayout->addWidget(&subDivLabel, 4, 1);
    frameLayout->addWidget(&subDivSlider, 5, 1);
    frameLayout->addWidget(&subDivValue, 5, 2);


    //frameLayout->addWidget(&label3, 4, 1);
    //frameLayout->addWidget(&enabled3, 5, 0);
    //frameLayout->addWidget(&option3, 5, 1);
    //frameLayout->addWidget(&value3, 5, 2);

    frameLayout->addWidget(&value4, 6, 0);
    frameLayout->addWidget(&label4, 7, 1);
    frameLayout->addWidget(&enabled4, 7, 0);

    frameLayout->setColumnMinimumWidth(2, 40);

    //setCentralWidget(frame);


    connect(&enabled1, SIGNAL(stateChanged(int)), this, SLOT(checked1(int)));
    connect(&enabled2, SIGNAL(stateChanged(int)), this, SLOT(checked2(int)));
    connect(&enabled3, SIGNAL(stateChanged(int)), this, SLOT(checked3(int)));

    connect(&option1, SIGNAL(valueChanged(int)), this, SLOT(updated1(int)));
    connect(&option2, SIGNAL(valueChanged(int)), this, SLOT(updated2(int)));
    connect(&option3, SIGNAL(valueChanged(int)), this, SLOT(updated3(int)));
    connect(&subDivSlider, SIGNAL(valueChanged(int)), this, SLOT(updatedSubdiv(int)));


    connect(&ok, SIGNAL(clicked()), this, SLOT(set()));
    connect(&cancel, SIGNAL(clicked()), this, SLOT(close()));

    

    texLayout = new QGridLayout();
    texLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    QFrame *f = new QFrame(this);
    f->setLayout(texLayout);
    texArea.setWidget(f);
    texArea.setFixedHeight(250);
    texArea.setFixedWidth(TEX_PER_ROW * TEX_DISPLAY_SIZE + 50);
    texArea.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QDir directory("./Resources/Textures/");
    textureList = directory.entryList(QStringList("*.jpg"));

    QToolButton *button;
    for (int i=0; i<textureList.count(); ++i)
    {
        button = new QToolButton(this);
        button->setIcon(QIcon("./Resources/Textures/" + textureList[i]));
        button->setIconSize(QSize(TEX_DISPLAY_SIZE-3,TEX_DISPLAY_SIZE-3));
        texGroup.addButton(button, i);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        texLayout->addWidget(button, i/TEX_PER_ROW, i%TEX_PER_ROW);
        button->setMinimumHeight(TEX_DISPLAY_SIZE);
        button->setMaximumHeight(TEX_DISPLAY_SIZE);
        button->setMinimumWidth(TEX_DISPLAY_SIZE);
        button->setMaximumWidth(TEX_DISPLAY_SIZE);

        button->setStyleSheet("QToolButton {border-width: 0px; border-color: black; border-style:solid; padding: 0px;}"
                              "QToolButton:checked {border-width: 3px; border-color: black; border-style:solid; padding: 0px;}");

    }

    if (textureList.size() > 0)
        texGroup.button(0)->setChecked(true);

}


OptionsDialog::~OptionsDialog()
{
    QList<QAbstractButton*> b = texGroup.buttons();
    for (int i=0; i<b.count(); ++i)
    {
        delete b[i];
    }

    delete texLayout;
    delete frameLayout;
    delete layout;
}

void OptionsDialog::showTextureDialog()
{
    textureDialog->exec();
}

void OptionsDialog::setValues(int v1, int v2, int v3, int v4, int v5, int subdivValue)
{
    if (v1 > 0)
    {
        enabled1.setChecked(true);
        option1.setValue(v1+1);
        option1.setValue(v1);
        value1.setEnabled(true);
    }
    else
    {
        enabled1.setChecked(false);
        option1.setValue(-v1+1);
        option1.setValue(-v1);
        value1.setEnabled(false);
    }

    if (v2 > 0)
    {
        enabled2.setChecked(true);
        option2.setValue(v2+1);
        option2.setValue(v2);
        value2.setEnabled(true);
    }
    else
    {
        enabled2.setChecked(false);
        option2.setValue(-v2+1);
        option2.setValue(-v2);
        value2.setEnabled(false);
    }

    if (v3 > 0)
    {
        enabled3.setChecked(true);
        option3.setValue(v3+1);
        option3.setValue(v3);
        value3.setEnabled(true);
    }
    else
    {
        enabled3.setChecked(false);
        option3.setValue(-v3+1);
        option3.setValue(-v3);
        value3.setEnabled(false);
    }
    subDivSlider.setValue(subdivValue + 1);
    subDivSlider.setValue(subdivValue - 1);
    subDivSlider.setValue(subdivValue);
  //      emit subDivSlider.valueChanged;
      //  subDivValue.setText();
        subDivValue.setEnabled(true);


    enabled4.setChecked(v4 != 0);

    if (texGroup.button(v5) != 0)
    {
        texGroup.button(v5)->setChecked(true);
    }
}


void OptionsDialog::checked1(int state)
{
    if (state == 0)
    {
        option1.setEnabled(false);
        value1.setEnabled(false);
    }
    else if (state == 2)
    {
        option1.setEnabled(true);
        value1.setEnabled(true);
    }
}

void OptionsDialog::checked2(int state)
{
    if (state == 0)
    {
        option2.setEnabled(false);
        value2.setEnabled(false);
    }
    else if (state == 2)
    {
        option2.setEnabled(true);
        value2.setEnabled(true);
    }
}

void OptionsDialog::checked3(int state)
{
    if (state == 0)
    {
        option3.setEnabled(false);
        value3.setEnabled(false);
    }
    else if (state == 2)
    {
        option3.setEnabled(true);
        value3.setEnabled(true);
    }
}

void OptionsDialog::updated1(int value)
{
    char val[64];
    sprintf(val,"%d",value);
    value1.setText(QString(val));
}

void OptionsDialog::updated2(int value)
{
    char val[64];
    sprintf(val,"%d",value);
    value2.setText(QString(val));
}

void OptionsDialog::updated3(int value)
{
    char val[64];
    sprintf(val,"%d",value);
    value3.setText(QString(val));
}

void OptionsDialog::updatedSubdiv(int value)
{
    char val[64];
    sprintf(val,"%d",value);
    subDivValue.setText(QString(val));
}



void OptionsDialog::set()
{
    int v1 = -option1.value();
    if (enabled1.isChecked())
        v1 = option1.value();

    int v2 = -option2.value();
    if (enabled2.isChecked())
        v2 = option2.value();

    int v3 = -option3.value();
    if (enabled3.isChecked())
        v3 = option3.value();

    int v4;
    if (enabled4.isChecked())
        v4 = 1;
    else
        v4 = 0;

    int v5 = texGroup.checkedId();

    emit valuesAccepted(v1, v2, v3, v4, v5, subDivSlider.value());
    this->done(0);
}

void OptionsDialog::close()
{
    this->done(0);
}
