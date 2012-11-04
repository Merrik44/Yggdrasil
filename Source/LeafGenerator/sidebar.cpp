#include "sidebar.h"

SideBar::SideBar(SketchArea *s, QWidget *parent) :
    QWidget(parent)
{
    sketchArea  = s;
    paramChooser = new ParamChooser();
    setMaximumWidth(200);

    //Draw
    QVBoxLayout *layout = new QVBoxLayout(this);
    draw = new QGroupBox(tr("Draw"), this);
    QVBoxLayout *drawLayout = new QVBoxLayout(draw);

    draw_outline = new QPushButton(tr("Draw Outline"),draw);
    draw_outline->setObjectName(tr("Draw Outline"));
    draw_root = new QPushButton(tr("Select Root"),draw);
    draw_root->setObjectName(tr("Select Root"));
    draw_eraser = new QPushButton(tr("Eraser"),draw);
    draw_eraser->setObjectName(tr("Eraser"));
    draw_validate = new QPushButton(tr("Validate"),draw);

    drawLayout->addWidget(draw_outline);
    drawLayout->addWidget(draw_root);
    drawLayout->addWidget(draw_eraser);
    drawLayout->addWidget(draw_validate);
    draw->setLayout(drawLayout);

    //Params
    params = new QGroupBox(tr("Parameters"), this);
    params->setEnabled(false);
    QVBoxLayout *paramsLayout = new QVBoxLayout(params);

    params_set = new QPushButton(tr("Set Parameters"),params);

    paramsLayout->addWidget(params_set);
    params->setLayout(paramsLayout);


    //Create
    create = new QGroupBox(tr("Create"), this);
    create->setEnabled(false);
    QVBoxLayout *createLayout = new QVBoxLayout(create);

    create_preview = new QPushButton(tr("Preview Leaf Structure"),create);
    create_number = new QSpinBox(this);
    create_number->setMinimum(1);
    create_number->setPrefix(tr("Create "));
    create_number->setSuffix(tr(" leaves"));
    create_ok = new QPushButton(tr("Create"),create);

    createLayout->addWidget(create_preview);
    createLayout->addWidget(create_number);
    createLayout->addWidget(create_ok);
    create->setLayout(createLayout);


    //Final layout
    layout->addWidget(draw);
    layout->addWidget(params);
    layout->addWidget(create);

    createActions();
}

void SideBar::reset()
{
    params->setEnabled(false);
    create->setEnabled(false);
}

void SideBar::validate()
{
    if(isValid())
    {
        params->setEnabled(true);
        //create->setEnabled(true);
    }
    else
    {
        params->setEnabled(false);
        create->setEnabled(false);
    }
}

void SideBar::paramsSet()
{
    paramChooser->show();
}

void SideBar::paramsOk()
{
    create->setEnabled(true);
}

void SideBar::create_okClicked()
{
    sketchArea->createLeaves(create_number->value());
}

void SideBar::createActions()
{
    connect(draw_outline, SIGNAL(clicked()), this, SLOT(drawClicked()));
    connect(draw_root, SIGNAL(clicked()), this, SLOT(drawClicked()));
    connect(draw_eraser, SIGNAL(clicked()), this, SLOT(drawClicked()));
    connect(draw_validate, SIGNAL(clicked()), this, SLOT(validate()));

    connect(this, SIGNAL(changeMode(QString)), sketchArea, SLOT(changeMode(QString)));
    connect(this, SIGNAL(isValid()), sketchArea, SLOT(validate()));

    connect(params_set, SIGNAL(clicked()), this, SLOT(paramsSet()));

    connect(create_ok, SIGNAL(clicked()),this,SLOT(create_okClicked()));

    connect(paramChooser, SIGNAL(returnValues(int,QPointF*,QPointF*,QPointF*,
                                      int,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,
                                      QPointF*,QPointF*,QPointF*,QPointF*,QPointF*,QPointF*,
                                      QPointF*, QColor*, QColor*, QPointF*)),
            sketchArea,SLOT(setValues(int,QPointF*,QPointF*,QPointF*,
                                              int,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,
                                              QPointF*,QPointF*,QPointF*,QPointF*,QPointF*,QPointF*,
                                              QPointF*, QColor*, QColor*, QPointF*)));

    connect(paramChooser, SIGNAL(returnValues(int,QPointF*,QPointF*,QPointF*,
                                      int,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,QColor*,
                                      QPointF*,QPointF*,QPointF*,QPointF*,QPointF*,QPointF*,
                                      QPointF*, QColor*, QColor*, QPointF*)),
            this,SLOT(paramsOk()));

}

void SideBar::drawClicked()
{
    QString modeStr = sender()->objectName();
    reset();
    changeMode(modeStr);
}
