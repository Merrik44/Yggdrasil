#ifndef SKETCHAREA_H
#define SKETCHAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QQueue>
//#include <QCursor>
#include "leaftemplate.h"

#include <QMessageBox>

class SketchArea : public QWidget
{
    Q_OBJECT
public:
    explicit SketchArea(QWidget *parent = 0);
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    bool isModified() const { return modified; }
    QColor getPenColor() const { return penColor; }

public slots:
    void clearImage();
    void print();
    void changeMode(QString modeStr);
    bool validate();
    void createLeaves(const int i);
    void setValues(int growthType, QPointF *hGrowthVar, QPointF *vGrowthVar, QPointF *scaleVar,
                   int colourType, QColor* fCol_MMin, QColor* fCol_MMax, QColor* fCol_AMin, QColor* fCol_AMax,
                   QColor* bCol_MMin, QColor* bCol_MMax, QColor* bCol_AMin, QColor* bCol_AMax,
                   QPointF *specVar, QPointF *bumpVar,
                   QPointF *spawnRateVar, QPointF *bsVar, QPointF *bvVar, QPointF *kdVar,
                   QPointF* intensityVar, QColor* rootColour_min, QColor* rootColour_max, QPointF* startScaleVar);

signals:
    void setStatusText(QString text);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawLineTo(const QPointF &endPoint);
    void paintRoot();
    void paintEraser(const QPointF &mouse);
    void resizeImage(QImage *image, const QSize &newSize);
    bool validateCheck(QPointF* p, QQueue<QPointF*>* queue, QList<QVector2D*>* edges);

    int mode; //0=draw leaf , 1=draw root, 2= erase
    bool modified;
    bool isMousePress;
    QColor penColor;
    QImage image;
    QImage last; //This is a backup image that keeps the un-validated sketch
    QPointF lastPoint;

    LeafTemplate *leafTemplate;
    QList<LeafModel*> leaves;
    QImage rootImage;
};

#endif // SKETCHAREA_H
