#include "foliagechooser.h"


FoliageChooser::FoliageChooser(QWidget *parent) :
    QWidget(parent)
{
    leaves = new QList<QString>();
    ratios = new QList<int>();
    createWidgets();
    createConnections();

    QGroupBox* leafBoxOutside = new QGroupBox(tr("Leaves For Foliage"));
    QScrollArea* leafArea = new QScrollArea();
    leafArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGroupBox* leafBox = new QGroupBox();
    QVBoxLayout *leafBoxLayout = new QVBoxLayout;

    QDir main = QDir(QString("Resources/Generated_Leaves/Leaf_Textures"));
    main.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList s = main.entryList();
    QStringListIterator i(s);

    while(i.hasNext())
    {
        int row = 0;
        int column = 0;
        QString group = i.next();
        QGroupBox* box = new QGroupBox(group);
        QGridLayout* layout = new QGridLayout();
        QDir groupFolder = QDir(main.path()+QString("/")+group);
        QStringList files;
        files<<"*.mtl";
        groupFolder.setNameFilters(files);
        files = groupFolder.entryList();

        QStringListIterator j(files);
        while(j.hasNext())
        {
            QString leaf  = j.next();
            leaf.chop(leaf.length()-leaf.lastIndexOf("."));
            LeafChooser* l = new LeafChooser(groupFolder.path()+QString("/")+leaf);
            leafOptions.append(l);
            layout->addWidget(l,row,column);
            connect(l,SIGNAL(leafMousedOver(QString)),this,SLOT(leafMouseOver(QString)));
            column++;
            if(column > 2)
            {
                column = 0;
                row++;
            }
        }
        box->setLayout(layout);
        leafBoxLayout->addWidget(box);
    }
    leafBoxLayout->addStretch();
    leafBox->setLayout(leafBoxLayout);
    leafArea->setWidget(leafBox);
    QHBoxLayout* outside = new QHBoxLayout();
    outside->addWidget(leafArea);
    leafBoxOutside->setLayout(outside);

    //REST------------------------------------------------------------|
    QGroupBox* preview = new QGroupBox(QString("Detail:                              Top                                                                           Bottom"));
    QHBoxLayout* preLayout = new QHBoxLayout();
    preLayout->addWidget(previewTop);
    preLayout->addWidget(previewBottom);
    preview->setLayout(preLayout);



    QGroupBox* growthBox = new QGroupBox(tr("Variables"));
    QVBoxLayout *growthBoxLayout = new QVBoxLayout;
    growthBoxLayout->addWidget(numberLeaves);
    growthBoxLayout->addWidget(leafSpread);
    growthBoxLayout->addWidget(scaleVar);
    growthBoxLayout->addWidget(preview);
    growthBoxLayout->addStretch();
    growthBox->setLayout(growthBoxLayout);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(okButton);
    bottomLayout->addWidget(closeButton);
    bottomLayout->addStretch();

    QHBoxLayout *midLayout = new QHBoxLayout;
    midLayout->addWidget(growthBox);
    midLayout->addWidget(leafBoxOutside);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(midLayout);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Set Foliage Parameters"));
    setFixedWidth(sizeHint().width());
    setFixedHeight(sizeHint().height());
}

void FoliageChooser::okClicked()
{
    QListIterator<LeafChooser*> optIt(leafOptions);
    while(optIt.hasNext())
    {
        LeafChooser* next = optIt.next();
        if(next->isSelected())
        {
            leaves->append(next->getName());
            ratios->append(next->getRatio());
        }
    }

    numberLeavesMin = numberLeaves->Min();
    numberLeavesMax = numberLeaves->Max();
    spreadMin = leafSpread->Min();
    spreadMax = leafSpread->Max();
    scaleMin = scaleVar->Min();
    scaleMax = scaleVar->Max();

    emit returnValues(leaves, ratios ,numberLeavesMin, numberLeavesMax,
                      spreadMin, spreadMax, scaleMin, scaleMax);

    close();
}


void FoliageChooser::createWidgets()
{
    okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    closeButton = new QPushButton(tr("&Cancel"));

    QString lst;
    QList<QPointF*> endpoints = QList<QPointF*>();

    QPixmap temp = QPixmap(300,300);
    temp.fill();
    previewTop = new QLabel();
    previewBottom = new QLabel();
    previewTop->setPixmap(temp);
    previewBottom->setPixmap(temp);

    QList<QString> leaves = QList<QString>();
    numberLeaves = new VariationChooser(tr("Leaves per Endpoint"),3,1,0, false);
    leafSpread = new VariationChooser(tr("Angle spread (degrees)"),20,0.1,2, false);
    scaleVar = new VariationChooser(tr("Leaf Scale"),10,1,0,false);
}

void FoliageChooser::createConnections()
{
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()),this, SLOT(close()));
}

void FoliageChooser::leafMouseOver(QString name)
{
    previewTop->setPixmap(QPixmap(name+QString("_top.png")).scaled(300,300));

    previewBottom->setPixmap(QPixmap(name+QString("_bottom.png")).scaled(300,300));
}
