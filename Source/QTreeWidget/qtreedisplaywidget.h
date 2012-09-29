#ifndef QTREEWIDGET_H
#define QTREEWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include "operation.h"
#include <list>
#include <string>


const std::string FileHeader = "LST 0.1";

class QTreeDisplayWidget : public QGLWidget
{
    Q_OBJECT

private:
    std::list<Operation*> renderList;
    std::string sourceFile;
    bool mouseDown;
    int movementX,movementY;
    float cameraRotationX,cameraRotationY;
    float cameraDistance;

public:
    QTreeDisplayWidget(QWidget *parent = 0);
    void setSourceFile(const char *fileName);
    void setSourceXMLFile(const char *fileName);

protected:
    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

    void mouseMoveEvent(QMouseEvent  *event);
    void mousePressEvent(QMouseEvent  *event);
    void mouseReleaseEvent(QMouseEvent  *event);
    void wheelEvent(QWheelEvent *event);
};

#endif
