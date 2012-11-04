#ifndef VARIATIONCHOOSER_H
#define VARIATIONCHOOSER_H

#include <QWidget>
#include <QtGui>

class VariationChooser : public QWidget
{
    Q_OBJECT
public:
    explicit VariationChooser(QString title, float defaultValue, float step, int numDecimals, bool centered=true, QWidget *parent = 0);
    QPointF* getValues();
    void setName(QString name);
    void setValues(float minVal, float maxVal);
    void setMinMax(float minVal, float maxVal);
    double Min();
    double Max();

signals:
    
public slots:

private slots:
    void minChanged(double value);

private:
    void setSpinBoxes(float defaultValue, float step, int numDecimals);

    QGroupBox *box;
    QDoubleSpinBox* min;
    QDoubleSpinBox* max;
};

#endif // VARIATIONCHOOSER_H
