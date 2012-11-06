#ifndef LEAFMODEL_H
#define LEAFMODEL_H

#include <QImage>
#include <QQueue>
#include <boost/math/common_factor.hpp>
#include <QPointF>
#include <qmath.h>
#include "leaftemplate.h"
#include "treenode.h"
#include <QVector>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>

#define LCM(a,b) boost::math::lcm(a,b)

class LeafModel
{
public:
    LeafModel(QList<QVector2D*>* edges, QPointF* root,
              int growthType, int hGrowth, int vGrowth, float scale,
              int colourType, QColor* baseColour, QColor* altColour,
              QColor* backBase, QColor* backAlt,
              float spec, float bump,
              float spawnRate, float bs, float bv, float kd,
              QString name, int intensity, QColor* rootColour, int startScale);
    QImage *getTopTexture();

private:
    QMessageBox messageBox;

    //Textures
    QImage *topTexture;
    QImage *bottomTexture;
    QImage *topBumpMap;
    QImage *bottomBumpMap;
    QImage* topAlpha;
    QImage* bottomAlpha;

    //Leaf Parameters
    QList<QVector2D*> *edges;
    QPointF *root;

    int growthType;
    enum GrowthTypes {ISOTROPIC_GROWTH, ANISOTROPIC_GROWTH, MARGINAL_GROWTH, NO_GROWTH};
    int hGrowth;
    int vGrowth;
    int steps; //Number of interations completed
    QList<QPointF*>* startValues; //Default from bounds = (20,20). This is the minimum size. If non-even growth, the fastest growing starts at 20, and the other is calculated as
                         //x0 = xmax - (h/v)(ymax-y0) (if x is the slowest growing)
    //float dy; //dy = (y-startValue[1])/vGrowth after vGrowth iterations, points end at original y.
    //float dx; //Same for x
    float area;

    float scale; //out of 100(?)

    int colourType; //Gives the way a leaf is coloured. 0= uniform, 2=from edge, 1=from veins
    enum ColourTypes {UNIFORM_COLOUR, FROM_VEINS_COLOUR, FROM_EDGE_COLOUR, SPECKLED_COLOUR};
    QColor* baseColour;
    QColor* altColour;

    QString name;
    int intensity;
    QColor* rootColour;
    int startScale;

    QColor* backBase;
    QColor* backAlt;

    float spec;
    float bump;

    //VEIN PARAMETERS
    float spawnRate; //Number of auxin per growth
    float bs; //Minimum distance from auxin at birth
    float bv; //Minimum distance from auxin at death
    float kd; //Kill distance (vein to auxin)

    //Vein structures
    QList<QPointF*> auxin;
    QList<TreeNode*> auxinTarget; //One closest vein per auxin

    TreeNode* rootNode;

    float bounds[4];
    //QImage base; //Should really be an array
    int size; //Image size (textures are square, at least to start)
    QVector<int> base;
    int marginalArea;

    //Collision detection
    int gridSize;
    int gridCount;
    QVector<QList<QPointF*>*> auxinGrid;
    QVector<QList<TreeNode*>*> veinGrid;

    float dx,dy;

    //Still needed, a way to store the mesh+uvs/ a way to store the entire model
    //Functions
    int getRand(int low, int high);
    float getRand(QPointF* p);
    QList<QVector2D*>* deepCopy(QList<QVector2D*> edges);
    QPointF* deepCopy(QPointF* root);
    QPointF* convertToPoint(QVector2D* v);
    void drawOutline(int vGrown, int hGrown);
    void fillImage(QImage image, QColor fill);
    void fillOutline(int fill);
    bool fillOutlineExplore(QPoint* point);
    bool fillOutlineValidate(QPoint* p, QQueue<QPoint*>* queue);

    QVector<int> drawMarginalOutline();
    void initialPlaceMarginalAuxin(QVector<int> marginalBase);
    void placeMarginalAuxin();

    void flood(int fill);

    void calcStartValues();

    void placeRoot();
    void shiftVeins(int steps);
    void shiftAuxin(int inc);
    void placeAuxin();
    void findClosest();
    TreeNode* findClosestVein(QPointF* point, int m, bool checkLOS);
    bool checkClosest(QPointF* p, TreeNode* node);
    TreeNode* quickFindClosestVein(QPointF* p);
    QPointF* findClosestAuxin(QPointF* point, int m);

    bool isInBounds(int x, int y, int m);
    bool isValid(int x, int y);
    bool isValidX(int x, int i);
    bool isValidY(int y, int i);

    void growVeins();
    void checkAuxin();
    void checkMarginalAuxin();
    bool checkAuxinTargets();
    void createVeinGrid();
    void createAuxinGrid();

    void drawVeins(QImage* image);
    QImage* drawVeinsOutline(QString name);

    void drawFromVein(QImage* image, TreeNode* node, QColor colour, int intensity);
    QImage* fromVeins(QImage* image, QColor altColor, int intensity);
    QImage* fillTexture(QImage* image, QColor colour);
    void drawFromEdge(QImage* image, QPointF* point, QColor colour, QColor bColour, int intensity);
    QImage* fromEdge(QImage* image, QColor altColor, QColor bColour, int intensity);
    void drawFromSpeckled(QImage* image, QPointF* point, QColor colour, int intensity);
    QImage* fromSpeckled(QImage* image, QColor altColor, int intensity);

    QImage resizeImage(QImage image);
    QImage *flipImage(QImage *image);

    QImage* drawTexture(QImage* texture, QColor* bColour, QColor* aColour);
    void drawPetiole(QImage* image);

    //DEBUG
    void drawDebugGrid(QPointF *p = NULL);
    void showMessage(QString message);
    void createMaterialFile(QString name, QString path);
    void createBumpMaps();

};

#endif // LEAFMODEL_H
