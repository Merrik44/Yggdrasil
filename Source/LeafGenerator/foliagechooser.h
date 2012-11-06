#ifndef FOLIAGECHOOSER_H
#define FOLIAGECHOOSER_H

#include <QWidget>
#include <QtGui>
#include <QList>
#include <QTableWidget>
#include <QString>
#include "variationchooser.h"
#include "foliageparameters.h"
#include "leafchooser.h"


class FoliageChooser : public QWidget
{
    Q_OBJECT
public:
    explicit FoliageChooser(FoliageParameters *f, QWidget *parent = 0);

signals:
    void returnValues(QList<QString>* leaves, QList<int>* ratios ,
                 int numberLeavesMin, int numberLeavesMax,
                 float spreadMin, float spreadMax, float scaleMin, float scaleMax);

private slots:
    void okClicked();
    void leafMouseOver(QString name);

private:
    void createWidgets(FoliageParameters* f);
    void createConnections();


    QPushButton *lstButton;

    QString lst;
    //QList<LSTLeafNode*> leafNodes;

    QLabel *label;
    QCheckBox *caseCheckBox;
    QCheckBox *backwardCheckBox;
    QPushButton *okButton;
    QPushButton *closeButton;
    QPushButton *leafButton;
    QPushButton *removeButton;

    QList<QString>* leaves;
    QList<int>* ratios;
    VariationChooser* numberLeaves;
    VariationChooser* leafSpread;
    VariationChooser* scaleVar;

    int numberLeavesMin;
    int numberLeavesMax;

    float spreadMin;
    float spreadMax;
    float scaleMin;
    float scaleMax;

    QList<LeafChooser*> leafOptions;
    QLabel* previewTop;
    QLabel* previewBottom;

};

#endif // FOLIAGECHOOSER_H
