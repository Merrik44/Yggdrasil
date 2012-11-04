#include "variationchooser.h"

VariationChooser::VariationChooser(QString title, float defaultValue, float step, int numDecimals, bool centered, QWidget *parent) :
    QWidget(parent)
{
    setSpinBoxes(defaultValue, step,numDecimals);
    box = new QGroupBox(title);
    QHBoxLayout* boxLayout = new QHBoxLayout();
    if(centered)
        boxLayout->addStretch();
    boxLayout->addWidget(min);
    boxLayout->addWidget(max);
    boxLayout->addStretch();
    box->setLayout(boxLayout);

    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->addWidget(box);
    setLayout(mainLayout);
}

void VariationChooser::setSpinBoxes(float defaultValue, float step, int numDecimals)
{
    min = new QDoubleSpinBox();
    min->setSingleStep(step);
    min->setDecimals(numDecimals);
    min->setMinimum(step);
    min->setMaximum(10000);
    min->setPrefix(tr("Min: "));
    min->setValue(defaultValue);
    connect(min,SIGNAL(valueChanged(double)),this,SLOT(minChanged(double)));
    min->setFixedHeight(25);

    max = new QDoubleSpinBox();
    max->setSingleStep(step);
    max->setDecimals(numDecimals);
    max->setMinimum(step);
    max->setMaximum(10000);
    max->setPrefix(tr("Max: "));
    max->setValue(defaultValue);
    max->setFixedHeight(25);
    connect(max,SIGNAL(valueChanged(double)),this,SLOT(minChanged(double)));
}

QPointF* VariationChooser::getValues()
{
    return new QPointF(min->value(),max->value());
}

void VariationChooser::minChanged(double value)
{
    //If min changed, and greater than max
    if((value == min->value()) && (value > max->value()))
    {
        max->setValue(value);
    }
    else if(value < min->value()) //Is definately max
    {
       min->setValue(value);
    }
}

void VariationChooser::setName(QString name)
{
    box->setTitle(name);
}

void VariationChooser::setValues(float minVal, float maxVal)
{
    max->setValue(maxVal);
    min->setValue(minVal);
}

void VariationChooser::setMinMax(float minVal, float maxVal)
{
   min->setMaximum(maxVal);
   min->setMinimum(minVal);

   max->setMaximum(maxVal);
   max->setMinimum(minVal);
}

double VariationChooser::Min()
{
    return min->value();
}

double VariationChooser::Max()
{
    return max->value();
}
