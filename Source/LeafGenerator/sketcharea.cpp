#include "sketcharea.h"

#include <QtGui>

SketchArea::SketchArea(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    modified = false;
    isMousePress = false;
    penColor = Qt::black;
    mode = 0;
    image = QImage(512,512,QImage::Format_RGB32);
    last = QImage(512,512,QImage::Format_RGB32);
    leafTemplate = new LeafTemplate();

    //Create the root image
    rootImage = QImage(15,15,QImage::Format_ARGB32);
    QRgb value = qRgba(0,255,255,255);
    for (int i = 0; i<15; i++)
    {
        rootImage.setPixel(i,6,value);
        rootImage.setPixel(6,i,value);
        rootImage.setPixel(i,7,value);
        rootImage.setPixel(7,i,value);
        rootImage.setPixel(i,8,value);
        rootImage.setPixel(8,i,value);
    }
    rootImage.setPixel(7,7,Qt::black);

    rootImage.save(tr("root"),".png");
    clearImage();
}

//Open Image
bool SketchArea::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    //Select pixel (0,0) as our background colour
    QColor bg = QColor(loadedImage.pixel(0,0));
    image.fill(QColor(255,255,255,255).rgb());

    for(int i = 0; i < loadedImage.width(); i++)
        for(int j = 0; j < loadedImage.height(); j++)
        {
            if(loadedImage.pixel(i,j) != bg.rgb())
            {
                image.setPixel(i,j,QColor(0,0,0,255).rgb());
            }
        }

    last = QImage(image);
    modified = false;
    update();
    return true;
}

//Close Image
bool SketchArea::saveImage(const QString &fileName, const char *fileFormat)
 {
     QImage visibleImage = image;
     //resizeImage(&visibleImage, size());

     if (visibleImage.save(fileName, fileFormat)) {
         modified = false;
         return true;
     } else {
         return false;
     }
 }

void SketchArea::setPenColor(const QColor &newColor)
{
    penColor = newColor;
}

void SketchArea::changeMode(QString modeStr)
{
    image = QImage(last);
    update();
    //Takes format from menu name
    //QString modeStr = action->data().toString();
    if(modeStr == "Draw Outline")
        mode = 0;
    else if(modeStr == "Select Root")
        mode = 1;
    else
        mode = 2;
}

void SketchArea::clearImage()
{
    image.fill(Qt::white);
    last.fill(Qt::white);
    modified = false;
    update();
}

bool SketchArea::validate()
{
    QList<QVector2D*>* edges = new QList<QVector2D*>();
    QQueue<QPointF*>* queue = new QQueue<QPointF*>();
    bool valid = true;
    last = QImage(image);
    if((leafTemplate->getRoot() != NULL))
    {
        queue->append(leafTemplate->getRoot());
        while(queue->count() > 0)
        {
           valid = validateCheck(queue->dequeue(), queue, edges) && valid;
           if(queue->size() > image.height()*image.width())
           {
               setStatusText(tr("Validation: Failed :Incorrect Image Format"));
               return false;
           }
        }
    }
    else
    {
        setStatusText(tr("Validation: Failed"));
        image = QImage(last);
        return false;
    }

    if(valid)
    {
        setStatusText(tr("Validation: Success"));
        leafTemplate->setEdgeArray(edges);
    }
    else
    {
        setStatusText(tr("Validation: Failed"));
        image = QImage(last);
    }
    update(image.rect());
    return valid;

}

bool SketchArea::validateCheck(QPointF* p, QQueue<QPointF*>* queue, QList<QVector2D*>* edges)
{
    //sleep(1);
    //Theres a leak in the leaf image
    if((p->y() >= image.height()-1) || (p->x() >= image.width()-1) || (p->x() <= 0) || (p->y() <= 0))
        return false;
    else
    {
        QRgb pix = image.pixel(p->toPoint());
        if(pix == QColor(Qt::white).rgb())
        {
            image.setPixel(p->x(),p->y(),QColor(Qt::green).rgb());
            queue->append(new QPointF(p->x(),p->y()-1));
            queue->append(new QPointF(p->x(),p->y()+1));
            queue->append(new QPointF(p->x()+1,p->y()));
            queue->append(new QPointF(p->x()-1,p->y()));
            return true;
        }
        else if(pix == QColor(Qt::black).rgb())//We don't check the border (to avoid overspill)
        {
            image.setPixel(p->toPoint(),QColor(Qt::red).rgb());
            QPointF adjusted = QPointF(p->x()-leafTemplate->getRoot()->x(),p->y()-leafTemplate->getRoot()->y());
            QVector2D* vec = new QVector2D(adjusted);
            edges->append(vec);
            return true;
        }
        else if(pix == QColor(Qt::red).rgb() || pix == QColor(Qt::green).rgb()) //Meaning it's been passed
        {
            return true;
        }
    }
    return true;
}

void SketchArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(mode == 0 || mode == 2)
        {
            lastPoint = event->pos();
            isMousePress = true;
        }
        else if (mode == 1)
        {
            leafTemplate->setRoot(event->pos());
            //Now we clear the old root, and set a new one
            update();
        }
    }
}

void SketchArea::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && isMousePress)
        drawLineTo(event->pos());
    //if(mode == 2)
        //paintEraser(event->pos());
}

void SketchArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isMousePress) {
        drawLineTo(event->pos());
        isMousePress = false;
    }
}

void SketchArea::paintEvent(QPaintEvent *event)
 {
     QPainter painter(this);
     QRect dirtyRect = event->rect();
     painter.drawImage(dirtyRect, image, dirtyRect);

     //Update root position
     paintRoot();
 }


void SketchArea::paintRoot()
 {
    QPointF *root = leafTemplate->getRoot();
    if(root != NULL)
    {
        root->setX(root->x() - 8);
        root->setY(root->y() - 8);
        QPainter painter(this);
        QRect rect = QRect(root->x()-10, root->y()-10, 25, 25);
        //QRect rect = QRect(5, 5, 20, 20);
        //painter.drawImage(rect, rootImage, rect);
        painter.drawImage(*root,rootImage);
        update(rect);
    }
 }

/*void SketchArea::paintEraser(const QPoint &mouse)
 {
    QPainter painter(this);
    painter.setPen(QPen(Qt::magenta, 1, Qt::SolidLine, Qt::RoundCap,
                               Qt::RoundJoin));
    painter.drawEllipse(mouse,5,5);
    update(mouse.x()-5,mouse.y()-5,mouse.x()+5,mouse.y()+5);
 }*/


void SketchArea::resizeEvent(QResizeEvent *event)
 {
     if (width() > image.width() || height() > image.height()) {
         int newWidth = qMax(width() + 128, image.width());
         int newHeight = qMax(height() + 128, image.height());
         //resizeImage(&image, QSize(newWidth, newHeight));
         update();
     }
     QWidget::resizeEvent(event);
 }

void SketchArea::drawLineTo(const QPointF &endPoint)
{
    if(mode != -1 && mode != 1) //Not blank or root
    {
        QPainter painter(&image);
        if(mode == 0)
            painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap,
                                Qt::RoundJoin));
        else if(mode == 2)
            painter.setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::RoundCap,
                                Qt::RoundJoin));
        painter.drawLine(lastPoint, endPoint);
        modified = true;

        int rad = 1;
        if(mode == 2)
            rad= 3;

        last = QImage(image); //Update backup
        update(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized()
                                         .adjusted(-rad, -rad, +rad, +rad));
        lastPoint = endPoint;
    }
}

void SketchArea::resizeImage(QImage *image, const QSize &newSize)
 {
     if (image->size() == newSize)
         return;

     if(image->size().width() > newSize.width() || image->size().height() > newSize.height())
         *image = image->scaled(newSize);



     int x= (newSize.width() - image->size().width())/2;
     int y= (newSize.height() - image->size().height())/2;

     QImage newImage(newSize, QImage::Format_RGB32);
     newImage.fill(Qt::white);
     QPainter painter(&newImage);
     painter.drawImage(QPoint(x, y), *image);
     *image = newImage;
 }

void SketchArea::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *printDialog = new QPrintDialog(&printer, this);
    if (printDialog->exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = image.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(image.rect());
        painter.drawImage(0, 0, image);
    }
#endif // QT_NO_PRINTER
}

void SketchArea::createLeaves(const int i)
{

    leaves = leafTemplate->createLeaves(i);
    setStatusText(tr("Leaf Creation: Success"));

}

void SketchArea::setValues(int growthType, QPointF *hGrowthVar, QPointF *vGrowthVar, QPointF *scaleVar,
               int colourType, QColor* fCol_MMin, QColor* fCol_MMax, QColor* fCol_AMin, QColor* fCol_AMax,
               QColor* bCol_MMin, QColor* bCol_MMax, QColor* bCol_AMin, QColor* bCol_AMax,
               QPointF *specVar, QPointF *bumpVar,
               QPointF *spawnRateVar, QPointF *bsVar, QPointF *bvVar, QPointF *kdVar,
               QPointF* intensityVar, QColor* rootColour_min, QColor* rootColour_max, QPointF* startScaleVar)
{
    leafTemplate->setVariables(growthType, hGrowthVar, vGrowthVar, scaleVar,
                               colourType, fCol_MMin, fCol_MMax, fCol_AMin, fCol_AMax,
                               bCol_MMin, bCol_MMax, bCol_AMin, bCol_AMax,
                               specVar, bumpVar,
                               spawnRateVar, bsVar, bvVar, kdVar,
                               intensityVar, rootColour_min, rootColour_max, startScaleVar
                               );
    setStatusText(tr("Parameters Set"));
}
