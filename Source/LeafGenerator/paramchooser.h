#ifndef PARAMCHOOSER_H
#define PARAMCHOOSER_H

#include <QDialog>
#include <QtGui>
#include "colourchooser.h"
#include "variationchooser.h"
#include "singlecolourchooser.h"

class ParamChooser : public QDialog
{
    Q_OBJECT
public:
    explicit ParamChooser(QWidget *parent = 0);
    
signals:
    void colourConstant(bool isConstant, QString type);
    void returnValues(int growthType, QPointF *hGrowthVar, QPointF *vGrowthVar, QPointF *scaleVar,
                      int colourType, QColor* fCol_MMin, QColor* fCol_MMax, QColor* fCol_AMin, QColor* fCol_AMax,
                      QColor* bCol_MMin, QColor* bCol_MMax, QColor* bCol_AMin, QColor* bCol_AMax,
                      QPointF *specVar, QPointF *bumpVar,
                      QPointF *spawnRateVar, QPointF *bsVar, QPointF *bvVar, QPointF *kdVar,
                      QPointF* intensityVar, QColor* rootColour_min, QColor* rootColour_max, QPointF* startScaleVar);
    
private slots:
    void okClicked();
    void colourTypeChanged(QString typeName);
    void growthTypeChanged(QString typeName);
    void saveFile();
    void loadFile();

private:
    void createWidgets();
    void createConnections();

   QLabel *label;
   QCheckBox *caseCheckBox;
   QCheckBox *backwardCheckBox;
   QPushButton *okButton;
   QPushButton *closeButton;
   QPushButton *saveButton;
   QPushButton *loadButton;

   QComboBox* growthType;
   VariationChooser* hGrowth;
   VariationChooser* vGrowth;

   VariationChooser* scale;

   QComboBox* colourType;
   ColourChooser* frontColour;
   ColourChooser* backColour;

   VariationChooser* spec;
   VariationChooser* bump;


   VariationChooser* spawnRate;
   VariationChooser* bs;
   VariationChooser* bv;
   VariationChooser* kd;

   SingleColourChooser* rootColour;
   VariationChooser* startScaleVar;
};

#endif // PARAMCHOOSER_H
