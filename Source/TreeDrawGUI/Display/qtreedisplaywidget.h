#ifndef QTREEDISPLAYWIDGET_H
#define QTREEDISPLAYWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
//#include "operation.h"
#include "treemodel.h"
#include <list>
#include <string>
#include "MeshGenerator/mesh.h"
#include "MeshGenerator/lstskeleton.h"
#include <QProgressDialog>


class QTreeDisplayWidget : public QGLWidget
{
    Q_OBJECT

private:
    //std::list<Operation*> renderList;
    TreeModel *tree;
    std::string sourceFile;
    int movementMode;
    int movementX,movementY;
    float cameraRotationX;
    float cameraRotationY;
    float cameraDistance;

    float centerX, centerY, centerZ;
    float boundX, boundY, boundZ;
    float panX, panY;
    float panFactor;
    float zoomFactor;

    int backgroundColour;
    GLuint barkTexture, leafTexture;


    bool isCurrent;

    Mesh* subdivisionSurface;
    Mesh* foliageMesh;

    LstSkeleton* lstGraph;

public:
    bool renderSubdivisionSurface, renderTexture, renderFoliage;
    QTreeDisplayWidget(QWidget *parent = 0);
    ~QTreeDisplayWidget();
    bool setSourceFile(const std::string &fileName);
    void setSourceXMLFile(const std::string &fileName);
    void clearDisplay();
    void exportCylinderModelToObj(const std::string &fileName);
    void exportMeshToObj(const string &fileName);

    void setBackgroundColour(int colour);
    void setBarkTexture(const QString &path);
     void setLeafTexture(const QString &path);

    void setOutOfDate();
    void setCurrent();

    void LoadLST( std::string filepath );
    void GenerateMeshFromLST(QProgressDialog *progressBar);
    void ApplySubdivisionToMesh( int numberOfSubdvisions,  QProgressDialog* progressBar );
    bool displayGeneratedMesh;
    void LoadFoliage(std::string filepath);


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
