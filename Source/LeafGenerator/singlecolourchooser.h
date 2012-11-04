#ifndef SINGLECOLOURCHOOSER_H
#define SINGLECOLOURCHOOSER_H


#include <QWidget>
#include <QtGui>
//#include

class SingleColourChooser : public QWidget
{
    Q_OBJECT
public:
    explicit SingleColourChooser(QString title, QWidget *parent = 0);
    //The return functions to get the values
    QColor* getMainMin();
    QColor* getMainMax();

signals:

public slots:

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

    QLabel* mainImage;

    QPixmap createHPixmap(QColor* min, QColor* max);
    QIcon createIcon(QColor* c);
};

#endif // SINGLECOLOURCHOOSER_H
