#ifndef COLOURCHOOSER_H
#define COLOURCHOOSER_H

#include <QWidget>
#include <QtGui>
#include "variationchooser.h"

class ColourChooser : public QWidget
{
    Q_OBJECT
public:
    explicit ColourChooser(QString title, QWidget *parent = 0);
    //The return functions to get the values
    QColor* getMainMin();
    QColor* getMainMax();
    QColor* getAltMin();
    QColor* getAltMax();
    QPointF* getIntensity();

signals:
    
public slots:
    //Sets whether the colouring is constant (ie, disables alternate value if true)
    void setConstant(bool value, QString type);

protected slots:
    void btnClicked();

protected:
    bool isConstant;
    QGroupBox *bottomBox;

    QPushButton* mainMaxBtn;
    QLabel* mainMaxImage;
    QColor* mainMax;
    QPushButton* mainMinBtn;
    QLabel* mainMinImage;
    QColor* mainMin;

    VariationChooser* intensity;

    QPushButton* altMaxBtn;
    QLabel* altMaxImage;
    QColor* altMax;
    QPushButton* altMinBtn;
    QLabel* altMinImage;
    QColor* altMin;

    QLabel* sideImage;
    QLabel* mainImage;
    QLabel* altImage;

    QPixmap createHPixmap(QColor* min, QColor* max);
    QPixmap createVPixmap();
    QIcon createIcon(QColor* c);
};

#endif // COLOURCHOOSER_H
