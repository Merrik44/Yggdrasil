#include "leafmodel.h"

LeafModel::LeafModel( QList<QVector2D*>* edges, QPointF* root,
                      int growthType, int hGrowth, int vGrowth, float scale,
                      int colourType, QColor* baseColour, QColor* altColour,
                      QColor* backBase, QColor* backAlt,
                      float spec, float bump,
                      float spawnRate, float bs, float bv, float kd,
                      QString name, int intensity, QColor* rootColour, int startScale)
{
    //Set Variables
    this->edges = deepCopy(*edges);
    this->root = deepCopy(root);

    this->growthType = growthType;
    this->hGrowth = hGrowth;
    this->vGrowth = vGrowth;

    this->scale = scale;

    this->colourType = colourType;
    this->baseColour = baseColour;
    this->altColour = altColour;

    this->backBase = backBase;
    this->backAlt = backAlt;

    this->spec = spec;
    this->bump = bump;

    this->spawnRate = spawnRate;
    this->bs = bs;
    this->bv = bv;
    this->kd = kd;

    this->name = name;
    this->intensity = intensity;
    this->rootColour = rootColour;
    this->startScale = startScale;

    steps = 0;

    size = 512;
    gridSize = 16; //Must be a factor of 2
    gridCount = size/gridSize; //Number of buckets

    base = QVector<int>(size*size,-1);     

    auxinGrid = QVector<QList<QPointF*>*>(gridCount*gridCount); //Divides the texture into slighty larger blocks
    veinGrid = QVector<QList<TreeNode*>*>(gridCount*gridCount);

    //Initialise lists
    for(int i = 0; i < gridCount; i++)
        for(int j = 0; j < gridCount; j++)
        {
            auxinGrid[i+j*(gridCount)] = new QList<QPointF*>();
            veinGrid[i+j*(gridCount)] = new QList<TreeNode*>();
        }

    calcStartValues();
    placeRoot();
    createVeinGrid();

    if(growthType == MARGINAL_GROWTH)
    {
        steps = hGrowth;
        vGrowth = hGrowth;
        initialPlaceMarginalAuxin(drawMarginalOutline());

    }
    else if(growthType == ISOTROPIC_GROWTH)
    {
        steps = hGrowth;
        vGrowth = hGrowth;
    }
    else if(growthType == NO_GROWTH)
    {
        steps = hGrowth;
        vGrowth = hGrowth;

        initialPlaceMarginalAuxin(drawMarginalOutline());
    }
    else if(growthType == ANISOTROPIC_GROWTH)
    {
        if(hGrowth < vGrowth)
            steps = hGrowth;
        else
            steps = vGrowth;
    }

    showMessage(QString("Creating veins"));
    qDebug()<<"|----------------Start Leaf Loop----------------|";
    for(int i=startScale*scale; i < steps*scale; i++)
    {
        if(growthType != NO_GROWTH)
            drawOutline(i, i);
        else
            drawOutline(steps*scale-1 ,steps*scale -1);

        if(growthType == ISOTROPIC_GROWTH || growthType == ANISOTROPIC_GROWTH)
        {
            shiftVeins(i);
            shiftAuxin(i);
            //Roots have shifted, so recreate grid
            createVeinGrid();
        }
        if(growthType == MARGINAL_GROWTH || growthType == NO_GROWTH)
            placeMarginalAuxin();
        else
            placeAuxin();

        findClosest();
        growVeins();
        if(growthType == ISOTROPIC_GROWTH || growthType == ANISOTROPIC_GROWTH)
        {
            checkAuxin();
        }
        else if(growthType == MARGINAL_GROWTH || growthType == NO_GROWTH)
        {
            checkMarginalAuxin();
        }
    }
    qDebug()<<"|----------------End Leaf Loop----------------|";
//    drawVeins();
    //qDebug()<<"|----------------Start Leaf While----------------|";
    //Now just clean up the left overs
    /*while(auxin.size() > 0 && checkAuxinTargets())
    {
        findClosest();
        growVeins();
        checkAuxin();
    }*/
    //qDebug()<<"|----------------End Leaf While----------------|";
    drawVeinsOutline(QString("veinoutline.png"));

    //Texturing
    //drawDebugGrid();
    showMessage(QString("Creating Textures"));
    topTexture = new QImage(size,size, QImage::Format_ARGB32);
    bottomTexture = new QImage(size,size, QImage::Format_ARGB32);
    topTexture->fill(QColor(0,0,0,0).rgba());
    bottomTexture->fill(QColor(0,0,0,0).rgba());

    topBumpMap = new QImage(size,size, QImage::Format_ARGB32);
    bottomBumpMap  = new QImage(size,size, QImage::Format_ARGB32);
    topBumpMap ->fill(QColor(0,0,0,0).rgba());
    bottomBumpMap ->fill(QColor(0,0,0,0).rgba());

    drawPetiole(topTexture);
    drawTexture(topTexture, baseColour, altColour);
    drawTexture(bottomTexture, backBase, backAlt);

    drawPetiole(bottomTexture);
    //resize textures
    //Don't resize, just resize UV's
    //Y adjusted for petiole
      /*QSize newSize = QSize(bounds[3]-bounds[2]+1,size-bounds[0]);

      QImage top = QImage(newSize, QImage::Format_ARGB32);
      top.fill(QColor(255,255,255,0).rgba());

      QImage bottom = QImage(newSize, QImage::Format_ARGB32);
      bottom.fill(QColor(255,255,255,0).rgba());

      for(int i = 0; i<= bounds[3] - bounds[2]; i++)
          for(int j = 0; j<= size - bounds[0]; j++)
          {
              top.setPixel(i,j,topTexture->pixel(bounds[2]+i,bounds[0]+j));
              bottom.setPixel(i,j,bottomTexture->pixel(bounds[2]+i,bounds[0]+j));
          }

    topTexture = new QImage(top);
    bottomTexture = new QImage(bottom);*/

    //flipImage(bottomTexture);
    createBumpMaps();
    bottomTexture = new QImage(bottomTexture->mirrored(true, false));
    bottomBumpMap = new QImage(bottomBumpMap->mirrored(true, false));


    // + QString::number(i);
     int i = 0;

     //For texturing reasons
     name = name.replace(QChar(' '),QChar('_'),Qt::CaseInsensitive);
     QDir(QString("Resources/Generated_Leaves/Leaf_Textures")).mkdir(name);
     //While there are still leaves from the same group
     while(QFile().exists(QString("Resources/Generated_Leaves/Leaf_Textures/")+name+QString("/")+name+QString("_")+QString::number(i)+QString("_top.png")))
     {
         i++;
     }
     topTexture->save(QString("Resources/Generated_Leaves/Leaf_Textures/")+name+QString("/")+name+QString("_")+QString::number(i)+QString("_top.png"), "png");
     bottomTexture->save(QString("Resources/Generated_Leaves/Leaf_Textures/")+name+QString("/")+name+QString("_")+QString::number(i)+QString("_bottom.png"), "png");

     topBumpMap->save(QString("Resources/Generated_Leaves/Leaf_Textures/")+name+QString("/")+name+QString("_")+QString::number(i)+QString("_topN.png"), "png");
     bottomBumpMap->save(QString("Resources/Generated_Leaves/Leaf_Textures/")+name+QString("/")+name+QString("_")+QString::number(i)+QString("_bottomN.png"), "png");

     createMaterialFile(name+QString("_")+QString::number(i), QString("Resources/Generated_Leaves/Leaf_Textures/")+name+QString("/"));

     messageBox.hide();
}

void LeafModel::showMessage(QString message)
{
    messageBox.setText(QString("Leaf Status:"));
    messageBox.setInformativeText(message);
    messageBox.setIcon(QMessageBox::NoIcon);
    messageBox.setStandardButtons(0);
    messageBox.show();
}

void LeafModel::placeRoot()
{
    rootNode = new TreeNode(root);
}

void LeafModel::shiftVeins(int steps)
{
    rootNode->shift(root,steps);
}

void LeafModel::shiftAuxin(int inc)
{
    QListIterator<QPointF*> auxIt(auxin);
    while(auxIt.hasNext())
    {
        QPointF* next = auxIt.next();
        next->setX(next->x()+ (next->x()-root->x())/(float)(inc+1));
        next->setY(next->y()+ (next->y()-root->y())/(float)(inc+1));
    }
}

void LeafModel::placeAuxin()
{
    //qDebug()<<"[placeAuxin]";
    //Taken to a very small power
    int numAuxin = (int)area*spawnRate*pow(10,-6);

    for(int i =0; i < numAuxin; i++)
    {
        //Now we throw darts
        float x = getRand(bounds[2],bounds[3]);
        float y = getRand(bounds[0],bounds[1]);

        //Our dart is at the very least inside the leaf
        while(base[(int)x+size*(int)y] == -1)
        {
            x = getRand(bounds[2],bounds[3]);
            y = getRand(bounds[0],bounds[1]);
        }
        //qDebug()<<"- Dart Thrown! ["<<x<<","<<y<<"]";
        //Test to see if too close to other auxin:
        QPointF* point;
        point = new QPointF(x,y);

        bool valid = true;
        createAuxinGrid(); //Auxin can die, so this has to be done every turn

        QPointF* closest;
        closest = findClosestAuxin(point, 1);
        //qDebug()<<"- Closest Auxin Found - Auxin Count: "<< auxin.size();
        if(closest != NULL)
        {
            //qDebug()<<"-- Auxin not null ["<<closest->x()<<","<<closest->y()<<"]";
            float dist = sqrt(pow(closest->x()-x, 2) + pow(closest->y()-y, 2));
            if(dist < bs)
            {
                valid = false;
                //qDebug()<<"-- Auxin too close dist: "<<dist<<"< bs: "<<bs<<"";
            }
        }

        if(valid)
        {
            //I do this because qt likes to not let go of variables??
            //checkLOS false, don't care about line of sight, makes it faster
            TreeNode* node = findClosestVein(point, 1, false);
            closest = node->position;
            //qDebug()<<"- Closest Vein Found ["<<closest->x()<<","<<closest->y()<<"]";

            float dist = sqrt(pow(closest->x()-x, 2) + pow(closest->y()-y, 2));
            if(dist < bv)
            {
                //qDebug()<<"-- Vein too close dist: "<<dist<<"< bv: "<<bv<<"";
                valid = false;
            }
        }
        //If it makes it this far
        if(valid)
        {
           auxin.append(point);
            //qDebug()<<"- Auxin Created! ["<<x<<","<<y<<"]";
        }
    }
}

void LeafModel::initialPlaceMarginalAuxin(QVector<int> marginalBase)
{
    qDebug()<<"[placeAuxin]";
    //Taken to a very small power
    //We place all the auxin for every step first
    int numAuxin = (int)marginalArea*spawnRate*pow(10,-6)*steps;

    for(int i =0; i < numAuxin; i++)
    {
        //Now we throw darts
        float x = getRand(bounds[2],bounds[3]);
        float y = getRand(bounds[0],bounds[1]);

        //Our dart is at the very least inside the leaf
        while(marginalBase[(int)x+size*(int)y] == -1)
        {
            x = getRand(bounds[2],bounds[3]);
            y = getRand(bounds[0],bounds[1]);
        }
        //qDebug()<<"- Dart Thrown! ["<<x<<","<<y<<"]";
        //Test to see if too close to other auxin:
        QPointF* point;
        point = new QPointF(x,y);

        bool valid = true;

        QPointF* closest;
        closest = findClosestAuxin(point, 1);
        //qDebug()<<"- Closest Auxin Found - Auxin Count: "<< auxin.size();
        if(closest != NULL)
        {
            //qDebug()<<"-- Auxin not null ["<<closest->x()<<","<<closest->y()<<"]";
            float dist = sqrt(pow(closest->x()-x, 2) + pow(closest->y()-y, 2));
            if(dist < bs)
            {
                valid = false;
                //qDebug()<<"-- Auxin too close dist: "<<dist<<"< bs: "<<bs<<"";
            }
        }
        //If it makes it this far
        if(valid)
        {
           auxinGrid[point->x()/gridSize + (point->y()/gridSize)*gridCount]->append(point);
            //qDebug()<<"- Auxin Created! ["<<x<<","<<y<<"]";
        }
    }
}

void LeafModel::placeMarginalAuxin()
{
    qDebug()<<"[placeAuxin]";
    //Taken to a very small power
    auxin.clear();
    for(int i = bounds[2]/gridSize; i <= bounds[3]/gridSize+1;i++)
        for(int j = bounds[0]/gridSize; j <= bounds[1]/gridSize+1;j++)
        {
            QListIterator<QPointF*> auxIt(*auxinGrid[i+j*gridCount]);
            while(auxIt.hasNext())
            {
                //If it has come into view of the outline
                QPointF* p = auxIt.next();
                if(base[p->x()+p->y()*size] == 0)
                {
                    //drawDebugGrid(p);
                    auxin.append(p);
                }
            }
        }
}

void LeafModel::createVeinGrid()
{
    //Clear lists
    for(int i = 0; i < gridCount; i++)
        for(int j = 0; j < gridCount; j++)
        {
            veinGrid[i+j*(gridCount)]->clear();
        }

    rootNode->addToGrid(veinGrid, gridCount, gridSize);
}

void LeafModel::createAuxinGrid()
{
    for(int i = 0; i < gridCount; i++)
        for(int j = 0; j < gridCount; j++)
        {
            auxinGrid[i+j*(gridCount)]->clear();
        }

    QListIterator<QPointF*> auxIt(auxin);
    while(auxIt.hasNext())
    {
        QPointF* p = auxIt.next();
        QPoint pos = p->toPoint();
        auxinGrid[pos.x()/gridSize + (gridCount)*(pos.y()/gridSize)]->append(p);
    }
}

//When checkLOS = false, never returns null. Otherwise, can return null
TreeNode* LeafModel::findClosestVein(QPointF* point, int m, bool checkLOS)
{
    //This function can return a null
    if(m > size && checkLOS)
        return NULL;

    //drawDebugGrid(point);
    //qDebug()<<"[findClosestVein(["<<point->x()<<","<<point->y()<<"]"<<m<<")]";

    TreeNode* node = NULL;
    float closest = 1000000000000;
    int x = point->toPoint().x()/gridSize;
    int y = point->toPoint().y()/gridSize;

    //Check p bucket
    if(m <= 1)
    {
        m = 1;
        if(isValid(x, y))
        {
            QListIterator<TreeNode*> nodeIt(*(veinGrid[x + (y)*(gridCount)]));
            while(nodeIt.hasNext())
            {
                //qDebug()<<"- hasNext";
                TreeNode* next = nodeIt.next();
                float dist = pow(next->position->x()-point->x(), 2) + pow(next->position->y()-point->y(), 2);
                //qDebug()<<"-- dist: "<<dist <<" ?= closest:"<<closest;
                if((node == NULL || dist < closest) && (!checkLOS || (checkLOS && checkClosest(point, next))))
                {
                    node = next;
                    //Debug()<<"--- Accepted["<<node->position->x()<<","<<node->position->y()<<"]";

                    closest = dist;
                }
            }
        }

    }

    //Check at m along x
    for(int i = -m; i <= m; i++)
    {
        //check [x+i, y+m]
        if(isValid(x+i, y+m))
        {
            QListIterator<TreeNode*> nodeIt(*(veinGrid[x+i + (y+m)*(gridCount)]));
            while(nodeIt.hasNext())
            {
                //qDebug()<<"- hasNext";
                TreeNode* next = nodeIt.next();
                float dist = pow(next->position->x()-point->x(), 2) + pow(next->position->y()-point->y(), 2);
                //qDebug()<<"-- dist: "<<dist <<" ?= closest:"<<closest;
                if((node == NULL || dist < closest) && (!checkLOS || (checkLOS && checkClosest(point, next))))
                {
                    node = next;
                    //qDebug()<<"--- Accepted["<<node->position->x()<<","<<node->position->y()<<"]";

                    closest = dist;
                }
            }
        }

        //check [x+i, y-m]
        if(isValid(x+i, y-m))
        {
            QListIterator<TreeNode*> nodeIt(*(veinGrid[x+i + (y-m)*(gridCount)]));
            while(nodeIt.hasNext())
            {
                //qDebug()<<"- hasNext";
                TreeNode* next = nodeIt.next();
                float dist = pow(next->position->x()-point->x(), 2) + pow(next->position->y()-point->y(), 2);
                //qDebug()<<"-- dist: "<<dist <<" ?= closest:"<<closest;
                if((node == NULL || dist < closest) && (!checkLOS || (checkLOS && checkClosest(point, next))))
                {
                    node = next;
                    //qDebug()<<"--- Accepted["<<node->position->x()<<","<<node->position->y()<<"]";

                    closest = dist;
                }
            }
         }
    }

    //Check at m-1 along y
    for(int i = -(m-1); i <= m-1; i++)
    {
        //check [x+m, y+i]
        if(isValid(x+m, y+i))
        {
            QListIterator<TreeNode*> nodeIt(*(veinGrid[x+m + (y+i)*(gridCount)]));
            while(nodeIt.hasNext())
            {
                //qDebug()<<"- hasNext";
                TreeNode* next = nodeIt.next();
                float dist = pow(next->position->x()-point->x(), 2) + pow(next->position->y()-point->y(), 2);
                //qDebug()<<"-- dist: "<<dist <<" ?= closest:"<<closest;
                if((node == NULL || dist < closest) && (!checkLOS || (checkLOS && checkClosest(point, next))))
                {
                    node = next;
                    //qDebug()<<"--- Accepted["<<node->position->x()<<","<<node->position->y()<<"]";

                    closest = dist;
                }
            }
        }

        //check [x-m, y+i]
        if(isValid(x-m, y+i))
        {
            QListIterator<TreeNode*> nodeIt(*(veinGrid[x-m + (y+i)*(gridCount)]));
            while(nodeIt.hasNext())
            {
                //qDebug()<<"- hasNext";
                TreeNode* next = nodeIt.next();
                float dist = pow(next->position->x()-point->x(), 2) + pow(next->position->y()-point->y(), 2);
                //qDebug()<<"-- dist: "<<dist <<" ?= closest:"<<closest;
                if((node == NULL || dist < closest) && (!checkLOS || (checkLOS && checkClosest(point, next))))
                {
                    node = next;
                    //qDebug()<<"--- Accepted["<<node->position->x()<<","<<node->position->y()<<"]";

                    closest = dist;
                }
            }
        }
    }

    //if not found
    if(node != NULL)
    {
        //qDebug()<<"- returned: ["<<node->position->x()<<","<<node->position->y()<<"]";
        return node;
    }
    else
        return findClosestVein(point, m+1, checkLOS);
    //check at m+1. For veins, there will always be at least one element (the root), so never return null
}

bool LeafModel::isValid(int x, int y)
{
    //if x within bounds of grid
    bool isBounds = (x) >= 0;
    isBounds &= (x) < gridCount;

    //and y within bounds of grid
    isBounds &= (y) >= 0;
    isBounds &= (y) < gridCount;

    //if(!isBounds)
        //qDebug()<<"!isValid"<<"Here: "<<x<<", "<<y<<" Min: "<<bounds[2]->x()/gridSize<<", Max:"<<bounds[3]->x()/gridSize;

    return isBounds;
}

bool LeafModel::isValidX(int x, int i)
{
    bool isBounds = (x+i) >= 0;
    isBounds &= (x+i) < gridCount;
    /*isBounds &= (x+i) >= bounds[2]->x()/gridSize;
    isBounds &= (x+i) <= bounds[3]->x()/gridSize;*/

    //if(!isBounds)
        //qDebug()<<"!isValidX"<<"Here: "<<x<<", "<<i<<" Min: "<<bounds[2]->x()/gridSize<<", Max:"<<bounds[3]->x()/gridSize;

    return isBounds;
}

bool LeafModel::isValidY(int y, int i)
{
    bool isBounds = (y+i) >= 0;
    isBounds &= (y+i) < gridCount;
    /*isBounds &= (y+i) >= bounds[0]->y()/gridSize;
    isBounds &= (y+i) <= bounds[1]->y()/gridSize;*/

    //if(!isBounds)
        //qDebug()<<"!isValidY"<<"Here: "<<y<<", "<<i<<" Min: "<<bounds[0]->y()/gridSize<<", Max:"<<bounds[1]->y()/gridSize;

    return isBounds;
}

TreeNode* LeafModel::quickFindClosestVein(QPointF* p)
{
    //This finds the LOS after the node is selected. Not as accurate, but much faster
    TreeNode* node = findClosestVein(p,1,false);
    /*if(checkClosest(p,node))
    {*/
        return node;
   /* }
    else
    {
        return NULL;
    }*/
}


//Line of sight
bool LeafModel::checkClosest(QPointF* p, TreeNode* node)
{

    //Bresenhams

    int x1 = node->position->toPoint().x();
    int y1 = node->position->toPoint().y();
    int x2 = p->toPoint().x();
    int y2 = p->toPoint().y();

    int delta_x(x2 - x1);
    // if x1 == x2, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;

    int delta_y(y2 - y1);
    // if y1 == y2, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;

    if(base[x1+size*y1] != 0)
        return false;

    if (delta_x >= delta_y)
    {
        // error may go below zero
        int error(delta_y - (delta_x >> 1));

        while (x1 != x2)
        {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= delta_x;
                y1 += iy;
            }
            // else do nothing

            error += delta_y;
            x1 += ix;

            if(base[x1+size*y1] != 0)
                return false;
        }
    }
    else
    {
        // error may go below zero
        int error(delta_x - (delta_y >> 1));

        while (y1 != y2)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= delta_y;
                x1 += ix;
            }
            // else do nothing

            error += delta_x;
            y1 += iy;

            if(base[x1+size*y1] != 0)
                return false;
        }
    }
   /* float x1 = node->position->toPoint().x();
    float y1 = node->position->toPoint().y();
    int x2 = p->toPoint().x();
    int y2 = p->toPoint().x();

    float dist = sqrt(pow(x2-x1,2)+pow(y2-y1,2));

    while(dist > 2)
    {
        dist = sqrt(pow(x2-x1,2)+pow(y2-y1,2));
        QVector2D vec(x2-x1,y2-y1);
        vec.normalize();
        x1+= vec.x();
        y1+= vec.y();
        if(base[(int)x1+size*(int)y1] != 0)
            return false;
    }*/

    return true;
}

QPointF* LeafModel::findClosestAuxin(QPointF* point, int m)
{
    //This function can return a null
    if(m > size)
        return NULL;

    QPointF* auxin = NULL;
    float closest = 1000000000000;
    int x = point->toPoint().x()/gridSize;
    int y = point->toPoint().y()/gridSize;

    //Check p bucket
    if(m <= 1)
    {
        m = 1;
        if(isValid(x, y))
        {
            QListIterator<QPointF*> auxIt(*(auxinGrid[x + (y)*(gridCount)]));
            while(auxIt.hasNext())
            {
                QPointF* next = auxIt.next();
                float dist = pow(next->x()-point->x(), 2) + pow(next->y()-point->y(), 2);
                if(auxin == NULL || dist < closest)
                {
                    auxin = next;
                    closest = dist;
                }
            }
        }

    }

    //Check at m along x
    for(int i = -m; i <= m; i++)
    {
        //check [x+i, y+m]
        if(isValid(x+i, y+m))
        {
            QListIterator<QPointF*> auxIt(*(auxinGrid[x+i + (y+m)*(gridCount)]));
            while(auxIt.hasNext())
            {
                QPointF* next = auxIt.next();
                float dist = pow(next->x()-point->x(), 2) + pow(next->y()-point->y(), 2);
                if(auxin == NULL || dist < closest)
                {
                    auxin = next;
                    closest = dist;
                }
            }
        }

        //check [x+i, y-m]
        if(isValid(x+i, y-m))
        {
            QListIterator<QPointF*> auxIt(*(auxinGrid[x+i + (y-m)*(gridCount)]));
            while(auxIt.hasNext())
            {
                QPointF* next = auxIt.next();
                float dist = pow(next->x()-point->x(), 2) + pow(next->y()-point->y(), 2);
                if(auxin == NULL || dist < closest)
                {
                    auxin = next;
                    closest = dist;
                }
            }
         }
    }

    //Check at m-1 along y
    for(int i = -(m-1); i <= m-1; i++)
    {
        //check [x+m, y+i]
        if(isValid(x+m, y+i))
        {
            QListIterator<QPointF*> auxIt(*(auxinGrid[x+m + (y+i)*(gridCount)]));
            while(auxIt.hasNext())
            {
                QPointF* next = auxIt.next();
                float dist = pow(next->x()-point->x(), 2) + pow(next->y()-point->y(), 2);
                if(auxin == NULL || dist < closest)
                {
                    auxin = next;
                    closest = dist;
                }
            }
        }

        //check [x-m, y+i]
        if(isValid(x-m, y+i))
        {
            QListIterator<QPointF*> auxIt(*(auxinGrid[x-m + (y+i)*(gridCount)]));
            while(auxIt.hasNext())
            {
                QPointF* next = auxIt.next();
                float dist = pow(next->x()-point->x(), 2) + pow(next->y()-point->y(), 2);
                if(auxin == NULL || dist < closest)
                {
                    auxin = next;
                    closest = dist;
                }
            }
        }
    }

    //if not found
    if(auxin != NULL)
        return auxin;
    else
        return findClosestAuxin(point, m+1);

}

void LeafModel::flood(int fill)
{
    for(int i =0; i<512;i++)
        for(int j=0; j<512;j++)
            base[i+size*j] = fill;
}


void LeafModel::findClosest()
{
    //qDebug()<<"[Find Closest]";
    QListIterator<QPointF*> auxIt(auxin);
    auxinTarget.clear();
    while(auxIt.hasNext())
    {
        QPointF* next = auxIt.next();
        //qDebug()<<"- Checking for next Aux Target ["<<next->x()<<","<<next->y()<<"]";

        //Checks for Line Of Sight
        //TreeNode* closest = findClosestVein(next,1, true);
        ///Takes a long time. For testing:
        TreeNode* closest = quickFindClosestVein(next);
        auxinTarget.append(closest);
    }

}

void LeafModel::growVeins()
{
    //qDebug()<<"[growVeins]";
    //Set up an array to tell us if we have arleady counted the node in question
    int i;
    bool isUsed[auxinTarget.size()];
    int usedCount = 0;
    //qDebug()<<"- UsedCount (Size"<<auxinTarget.size()<<")";
    for(i=0; i< auxinTarget.size();i++)
    {
        QListIterator<TreeNode*> nodeIt(auxinTarget);
        if(nodeIt.next() != NULL)
            isUsed[i] = false;
        else //Not used so much as unusable :)
        {
            usedCount++;
            //qDebug()<<"-- AuxTarget["<<i<<"] = NULL UsedCount = "<<usedCount;
            isUsed[i] = true;
        }
    }

    //If all auxin are inaccessible
     //qDebug()<<"- Are all used?";
    /*if(usedCount == auxinTarget.size())
    {
        auxinTarget.clear();
        //Kill everything
        auxin.clear();
        //qDebug()<<"-- Yes, kill all. [usedCount:"<<usedCount<<" == auxTarget"<<auxinTarget.size()<<"]";
    }*/

    i = -1;
    QListIterator<TreeNode*> nodeIt(auxinTarget);

    //qDebug()<<"- Calc Vectors";
    while (nodeIt.hasNext())
    {
        QList<QVector2D> auxVec;
        TreeNode* next = nodeIt.next();
        i++;
         //qDebug()<<"-- Target "<<i;
        if(!isUsed[i])
        {
             //qDebug()<<"--- Not used, finding similar";
            QListIterator<TreeNode*> nodeIt2(auxinTarget);
            QListIterator<QPointF*> auxIt(auxin);
            int j=-1;
            while(nodeIt2.hasNext())
            {
                TreeNode* next2 = nodeIt2.next();
                QPointF* aux = auxIt.next();
                j++;
                if(next2 == next && next != NULL)
                {
                    //qDebug()<<"--- Similar found! ["<<j<<"]";
                    isUsed[j] = true;
                    auxVec.append(QVector2D(aux->x()-next2->position->x(),aux->y()-next2->position->y()));
                    if(growthType = MARGINAL_GROWTH && auxVec.size() > 5)
                        break;
                }
            }

             //qDebug()<<"-- Summing Vectors";
            //Now that we have all the connections
             QVector2D vec(0,0);
             QListIterator<QVector2D> vecIt(auxVec);
             while(vecIt.hasNext())
             {
                 QVector2D nextVec = vecIt.next();
                 nextVec.normalize();
                 vec += nextVec;
             }

             float distance = 1;
              //qDebug()<<"-- Averaged Vector Length ["<<vec.length()<<"]";
             //So we need to do a small check here. If |vec| < 1,
             //Then normalising it will make it further away than the previous closest
             //and cause it to be picked again, casuing an infinite loop. SO to stop this, we just split the vectors and recalculate
             while(vec.length() < 1 && auxVec.size() > 0)
             {
                  //qDebug()<<"--- Vector Too Small";
                 vec = QVector2D(0,0);
                 QVector2D vec2(0,0);
                 //Split first one off
                 //qDebug()<<"--- Vec2 Removed";
                 vec2 = auxVec.takeFirst();
                 //qDebug()<<"---- Vec2 ["<<vec2.x()<<","<<vec2.y()<<"]";
                 if(vec2.length() > 1)
                 {
                    vec2.normalize();
                 }
                 //And add it as a node
                 QPointF* p = new QPointF(next->position->x()+vec2.x(),next->position->y()+vec2.y());
                 TreeNode* t = new TreeNode(p,next);
                 veinGrid[p->x()/gridSize + gridCount*(p->y()/gridSize)]->append(t);
                 next->children->append(t);
                 //qDebug()<<"--- New Node Created on Vec2 ["<<p->x()<<","<<p->y()<<"] Parent: ["<<next->position->x()<<","<<next->position->y()<<"]";

                 QListIterator<QVector2D> vecIt(auxVec);
                 //Then continue as before
                 while(vecIt.hasNext())
                 {
                     QVector2D nextVec = vecIt.next();
                     nextVec.normalize();
                     vec += nextVec;
                 }
                 //qDebug()<<"--- New Vec |"<<vec.length()<<"|";
             }

             if(vec.length() >= 1)
             {

                 vec.normalize();
                 //And finally, we add a new node to the root tree

                 //Adding this step to prevent duplicate cycle
                 //if node alreadt exists, add as child to that node
                 QPointF* p = new QPointF(next->position->x()+vec.x(),next->position->y()+vec.y());
                 TreeNode* t = new TreeNode(p,next);
                 veinGrid[p->x()/gridSize + gridCount*(p->y()/gridSize)]->append(t);
                 next->children->append(t);

                 //qDebug()<<"-- Vec Added ["<<t->position->x()<<","<<t->position->y()<<"] parent["<<next->position->x()<<","<<next->position->y()<<"]";
             }
        }
    }
}

void LeafModel::checkAuxin()
{
    //qDebug()<<"[checkAuxin]";
    QListIterator<QPointF*> auxIt(auxin);
    //If an auxin dies, we don't transfer it
    QList<QPointF*> newAuxin;

    while(auxIt.hasNext())
    {
        QPointF* next = auxIt.next();
        //We only need one node within kill distance. We already know that no auxin can kill another (defined by insertion)
        //qDebug()<<"- Checking Closest Vein from ["<<next->x()<<","<<next->y()<<"]";
        QPointF* closest = findClosestVein(next, 1, false)->position;
        float dist = sqrt(pow(next->x()-closest->x(), 2) + pow(next->y()-closest->y(), 2));
        //qDebug()<<"-- Closest found ["<<closest->x()<<","<<closest->y()<<"]";
        //qDebug()<<"-- dist: "<<dist<<" ?= "<<kd<<"]";
        if(dist > kd) //So it's alive
        {
            //qDebug()<<"-- Yes, kept";
            newAuxin.append(next);
        }
    }
    auxin = newAuxin;
}

void LeafModel::checkMarginalAuxin()
{
    //qDebug()<<"[checkAuxin]";
    QListIterator<QPointF*> auxIt(auxin);
    //If an auxin dies, we remove from grid

    while(auxIt.hasNext())
    {
        QPointF* next = auxIt.next();
        //We only need one node within kill distance. We already know that no auxin can kill another (defined by insertion)
        QPointF* closest = findClosestVein(next, 1, false)->position;
        float dist = sqrt(pow(next->x()-closest->x(), 2) + pow(next->y()-closest->y(), 2));
        //qDebug()<<"-- Closest found ["<<closest->x()<<","<<closest->y()<<"]";
        //qDebug()<<"-- dist: "<<dist<<" ?= "<<kd<<"]";
        if(dist < kd) //So it's dead
        {
            //qDebug()<<"-- Nope, dead";
            //We reomve one, since there will only be one and this is quicker
             auxinGrid[next->x()/gridSize + (next->y()/gridSize)*gridCount]->removeOne(next);
        }
    }
}

void LeafModel::drawVeins(QImage *image)
{
    //QPainter p = QPainter(&veinImage);

    rootNode->draw(image, QColor(0,0,0,255));
    image->save(QString("Veins.png"), "png");
}

int LeafModel::getRand(int low, int high)
{
    return qrand()%(high+1-low)+low;
}

float LeafModel::getRand(QPointF* p)
{
    return p->x() + (float)qrand()/((float)RAND_MAX/(p->y()-p->x()));
}

//Just keeps this code out of the main function and makes it reusable
QPointF* LeafModel::convertToPoint(QVector2D *v)
{
    QPointF* p = new QPointF();//v->x()*(hCount/hGrowth)*scale, v->y()*(vCount/vGrowth)*scale);

    return p;
}

void LeafModel::calcStartValues()
{
    startValues = new QList<QPointF*>();

    QListIterator<QVector2D*> edgeIt(*edges);
    while(edgeIt.hasNext())
    {
        QVector2D* v = edgeIt.next();
        QPointF* p = new QPointF();
        //If the growth factors are constant and equal
        if(growthType == MARGINAL_GROWTH || growthType == ISOTROPIC_GROWTH)
        {
            vGrowth = hGrowth;
            //compute growth speeds
            dx = v->x()/(float)(hGrowth+1);
            dy = v->y()/(float)(vGrowth+1);
            //now, to make starting position
            p->setX(dx+root->x()); //(start at one growth)
            p->setY(dy+root->y());
            startValues->append(p);
        }
        else if(growthType == NO_GROWTH)//Constant size
        {
            vGrowth = hGrowth;
            //compute growth speeds
            dx = v->x()/(float)(hGrowth+1);
            dy = v->y()/(float)(vGrowth+1);
            //now, to make starting position
            p->setX(dx+root->x()); //(start at one growth)
            p->setY(dy+root->y());
            startValues->append(p);
        }
        else if(growthType == ANISOTROPIC_GROWTH)
        {
            //compute growth speeds
            dx = v->x()/(float)(hGrowth+1);
            dy = v->y()/(float)(vGrowth+1);

            //x grows slower than y
            if(hGrowth > vGrowth)
            {
                float dxNew = dx*(hGrowth - vGrowth); //how far it gets when y finishes
                p->setX(dx+root->x()+p->x()-dxNew); //(start at one growth)
                p->setY(dy+root->y());
                //hGrowth = vGrowth;
                startValues->append(p);
            }
            else //y grows slower
            {
                float dyNew = dy*(vGrowth - hGrowth); //how far it gets when x finishes
                p->setX(dx+root->x()); //(start at one growth)
                p->setY(dy+root->y()+p->y()-dyNew);
                //vGrowth = hGrowth;
                startValues->append(p);
            }
        }
    }
}

QList<QVector2D*>* LeafModel::deepCopy(QList<QVector2D*> edges)
{
    QList<QVector2D*>* newEdges = new QList<QVector2D*>();
    QListIterator<QVector2D*> edgesIterator(edges);

    while(edgesIterator.hasNext())
    {
        QVector2D* v = edgesIterator.next();
        newEdges->append(new QVector2D(v->x(),v->y()));
    }
    return newEdges;
}


QPointF* LeafModel::deepCopy(QPointF* root)
{
    return new QPointF(root->x(),root->y());
}

void LeafModel::drawOutline(int vGrown, int hGrown)
{
    flood(-1);
    QListIterator<QVector2D*> edgesIt(*edges);
    QListIterator<QPointF*> startIt(*startValues);
    QPointF* o = new QPointF(edges->first()->x()*hGrown/(float)hGrowth+startValues->first()->x(),
                             edges->first()->y()*vGrown/(float)vGrowth+startValues->first()->y());
    bounds[0] = o->y();
    bounds[1] = o->y();
    bounds[2] = o->x();
    bounds[3] = o->x();

    //First, draw in vector edges
    while(edgesIt.hasNext())
    {
        QVector2D* v = edgesIt.next();
        QPointF* p = startIt.next();
        QPoint* p2 = new QPoint(int(v->x()*hGrown/(float)hGrowth+p->x()),int(v->y()*vGrown/(float)vGrowth+p->y()));

        if(p2->y() < bounds[0]) //min y
            bounds[0] = p2->y();
        else if(p2->y() > bounds[1]) //max y
            bounds[1] = p2->y();
        else if(p2->x() < bounds[2]) //min x
            bounds[2] = p2->x();
        else if(p2->x() > bounds[3]) // max x
            bounds[3] = p2->x();

        base[p2->x()+p2->y()*size] = 1;
    }
    //Next, fill interior
    fillOutline(0);
}

QVector<int> LeafModel::drawMarginalOutline()
{
    int val = steps-1;
    QVector<int> marginalBase;
    marginalBase = QVector<int>(size*size,-1);

    QListIterator<QVector2D*> edgesIt(*edges);
    QListIterator<QPointF*> startIt(*startValues);
    QPointF* o = new QPointF(edges->first()->x()*val/(float)hGrowth+startValues->first()->x(),
                             edges->first()->y()*val/(float)vGrowth+startValues->first()->y());
    bounds[0] = o->y();
    bounds[1] = o->y();
    bounds[2] = o->x();
    bounds[3] = o->x();

    //First, draw in vector edges
    while(edgesIt.hasNext())
    {
        QVector2D* v = edgesIt.next();
        QPointF* p = startIt.next();
        QPoint* p2 = new QPoint(int(v->x()*val/(float)hGrowth+p->x()),int(v->y()*val/(float)vGrowth+p->y()));

        marginalBase[p2->x()+p2->y()*size] = 1;

        if(p2->y() < bounds[0]) //min y
            bounds[0] = p2->y();
        else if(p2->y() > bounds[1]) //max y
            bounds[1] = p2->y();
        else if(p2->x() < bounds[2]) //min x
            bounds[2] = p2->x();
        else if(p2->x() > bounds[3]) // max x
            bounds[3] = p2->x();
    }
    //drawDebugGrid(marginalBase);
    //Next, fill interior
    QQueue<QPoint> queue = QQueue<QPoint>();
    QPoint p3;
    queue.append(root->toPoint());

    marginalArea = 0;
    while(queue.size() > 0)
    {
        p3 = queue.dequeue();

        if(marginalBase[p3.x()+p3.y()*size] == -1)
        {
            marginalArea++;
            marginalBase[p3.x()+p3.y()*size] = 0;
            queue.append(QPoint(p3.x(),p3.y()-1));
            queue.append(QPoint(p3.x(),p3.y()+1));
            queue.append(QPoint(p3.x()+1,p3.y()));
            queue.append(QPoint(p3.x()-1,p3.y()));
        }

    }
    return marginalBase;
}

void LeafModel::fillOutline(int fill)
{
    QQueue<QPoint> queue = QQueue<QPoint>();
    QPoint p;

    //This is broken, but importnat for marginal growth
   /* //If the resolution is so small that the point is an edge
    if(base[root->toPoint().x(),root->toPoint().y()] != 0)
    {
        //Rerun the validate function in bounds to try and find an area to start
        for(int i=bounds[2]+1; i<bounds[3];i++)
            for(int j=bounds[0]+1; j<bounds[1];j++)
            {
                if(fillOutlineExplore(new QPoint(i,j)))
                    break;
            }
    }
    else
    {*/
        queue.append(root->toPoint());

        area = 0;

        while(queue.size() > 0)
        {
            p = queue.dequeue();

            if(base[p.x()+p.y()*size] == -1)
            {
                area++;
                base[p.x()+p.y()*size] = fill;
                queue.append(QPoint(p.x(),p.y()-1));
                queue.append(QPoint(p.x(),p.y()+1));
                queue.append(QPoint(p.x()+1,p.y()));
                queue.append(QPoint(p.x()-1,p.y()));
            }

        }
    //}
}

bool LeafModel::fillOutlineExplore(QPoint* point)
{
    QQueue<QPoint*>* queue = new QQueue<QPoint*>();
    bool valid = true;
    area = 0;
    QVector<int> last = QVector<int>(base);

    queue->append(point);
    while(queue->count() > 0)
    {
       valid = fillOutlineValidate(queue->dequeue(), queue) && valid;
       if(queue->size() > (bounds[1]-bounds[0])*(bounds[3]-bounds[2]))
       {

           return false;
       }
    }
    if(area == 0)
    {
        base = QVector<int>(last);
        return false;
    }
    else
    {
        //drawDebugGrid(new QPointF(point->x(),point->y()));
    }

    return valid;
}

bool LeafModel::fillOutlineValidate(QPoint* p, QQueue<QPoint*>* queue)
{
    if((p->y() >= bounds[1]) || (p->x() >= bounds[3]) || (p->x() <= bounds[2]) || (p->y() <= bounds[0]))
        return false;
    else
    {
        if(base[p->x()+p->y()*size] == -1)
        {
            area++;
            base[p->x()+p->y()*size] = 0;
            queue->append(new QPoint(p->x(),p->y()-1));
            queue->append(new QPoint(p->x(),p->y()+1));
            queue->append(new QPoint(p->x()+1,p->y()));
            queue->append(new QPoint(p->x()-1,p->y()));
        }
    }

    return true;
}

QImage* LeafModel::drawTexture(QImage* texture, QColor* bColour, QColor* aColour)
{
    fillTexture(texture, *bColour);

    QColor colour(*bColour);

    if(colourType != FROM_EDGE_COLOUR)
    {
        if(bColour->value() < 240)
            colour = colour.fromHsv(colour.hue(),colour.saturation(),colour.value() +10);
        else
            colour = colour.fromHsv(colour.hue(),colour.saturation(),colour.value());
        fromEdge(texture, colour ,*bColour, 6);
    }

    if(colourType != FROM_VEINS_COLOUR)
    {
        if(bColour->value() > 13)
            colour = colour.fromHsv(colour.hue(),colour.saturation(),colour.value() - 10);
        else
            colour = colour.fromHsv(colour.hue(),colour.saturation(),colour.value());
        fromVeins(texture, colour, 2);
    }

    if(colourType != SPECKLED_COLOUR)
    {
        if(bColour->value() > 7) //&& baseColour->hue() > 10)
            colour = colour.fromHsv(colour.hue(),colour.saturation(),colour.value() - 5);
        else
            colour = colour.fromHsv(colour.hue(),colour.saturation(),colour.value());
        fromSpeckled(texture, colour, 10);
    }

    if(colourType == FROM_EDGE_COLOUR)
    {
        fromEdge(texture, *aColour ,*bColour, intensity);
    }
    else if(colourType == FROM_VEINS_COLOUR)
    {
        fromVeins(texture, *aColour, intensity);
    }
    else if(colourType == SPECKLED_COLOUR)
    {
        fromSpeckled(texture, *aColour, intensity);
    }

    rootNode->draw(texture, *rootColour);
    //Optimise Texture
    resizeImage(*texture);
}

QImage LeafModel::resizeImage(QImage image)
 {
   //Y adjusted for petiole
     QSize newSize = QSize(bounds[3]-bounds[2]+1,bounds[1]-bounds[0]+21);

     /*if (image->size() == newSize)
         return image;

     QImage* newImage = new QImage(newSize, QImage::Format_ARGB32);
     newImage->fill(QColor(255,255,255,0).rgba());

     for(int i = 0; i<= bounds[3] - bounds[2]; i++)
         for(int j = 0; j<= bounds[1] - bounds[0]; j++)
         {
             newImage->setPixel(i,j,image->pixel(bounds[2]+i,bounds[0]+j));
         }

     image = new QImage(*newImage);*/


     return image;
 }

QImage* LeafModel::flipImage(QImage* image)
 {
     QImage* newImage = new QImage(image->size(), QImage::Format_ARGB32);
     newImage->fill(QColor(255,255,255,0).rgba());

     for(int i = 0; i<image->width(); i++)
         for(int j = 0; j <image->height(); j++)
         {
             newImage->setPixel(image->width() - i,j,image->pixel(i,j));
         }

     image = newImage;
     return newImage;
 }


void LeafModel::drawPetiole(QImage* image)
{
    QPainter p;
    p.begin(image);

    p.setPen(QPen(rootColour->toRgb(), rootNode->veinSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    QPainterPath path;
    //Create a bezier stalk, with slight sway (both points are [root.x+small rand, middle of line]
    QPointF c = QPoint(root->x(),(root->y()+image->height())/2);
    QPointF c1 = QPoint(getRand(new QPointF(12,22))+c.x(),c.y());
    QPointF c2 = QPoint(-getRand(new QPointF(12,22))+c.x(),c.y());
    path.moveTo(*root);
    path.cubicTo(c1,c2, QPoint(root->x(),image->height()));
    p.drawPath(path);
    //p.drawLine(rootNode->position->toPoint(), QPoint(rootNode->position->x(),bottom->height()));
    p.end();
}

void LeafModel::fillImage(QImage image, QColor fill)
{
    QQueue<QPoint> queue = QQueue<QPoint>();
    QPoint p;
    queue.append(root->toPoint());
    QRgb pix;
    while(queue.size() > 0)
    {
        p = queue.dequeue();
        pix = image.pixel(p);

        if(pix == QColor(255,255,255,255).rgba())
        {
            image.setPixel(p,fill.rgba());
            queue.append(QPoint(p.x(),p.y()-1));
            queue.append(QPoint(p.x(),p.y()+1));
            queue.append(QPoint(p.x()+1,p.y()));
            queue.append(QPoint(p.x()-1,p.y()));
        }
    }
//    base.save(QString("root"), "png");
}

QImage* LeafModel::getTopTexture()
{
    return topTexture;
}

QImage* LeafModel::fillTexture(QImage* image, QColor colour)
{
    //image->fill(QColor(0,0,0,0));

    QQueue<QPoint> queue = QQueue<QPoint>();
    QPoint p;
    QVector<int> outline = QVector<int>(base);
    queue.append(root->toPoint());
    while(queue.size() > 0)
    {
        p = queue.dequeue();

        if(outline[p.x()+p.y()*size] == 0)
        {
            image->setPixel(p,colour.rgba());
            outline[p.x()+p.y()*size] = 1;
            queue.append(QPoint(p.x(),p.y()-1));
            queue.append(QPoint(p.x(),p.y()+1));
            queue.append(QPoint(p.x()+1,p.y()));
            queue.append(QPoint(p.x()-1,p.y()));
        }
    }
    return image;
}

QImage* LeafModel::fromVeins(QImage* image, QColor altColor, int intensity)
{
    TreeNode* node = rootNode;
    drawFromVein(image, node, altColor, intensity);
    return image;
}

QImage* LeafModel::fromEdge(QImage* image, QColor altColor, QColor bColour, int intensity)
{
    QPointF p;
    QListIterator<QVector2D*> edgeIt(*edges);
    while(edgeIt.hasNext())
    {
        p = edgeIt.next()->toPointF();
        drawFromEdge(image, &p, altColor, bColour, intensity);
    }
   return image;
}

void LeafModel::drawFromEdge(QImage* image, QPointF* point, QColor colour, QColor bColour, int intensity)
{
    QPoint p = point->toPoint();
    p.setX(p.x()+root->x());
    p.setY(p.y()+root->y());

    float max = intensity;//sqrt(2*pow(intensity, 2)); //Max distance
    //alt colour full at p, base colour full outside p
    for(int i = -(intensity);i < (intensity); i++)
       for(int j = -(intensity);j < (intensity); j++)
       {
            float weight = sqrt(pow(i, 2) + pow(j, 2))/max;

            if(weight > 1)
                weight = 1;
            int h = (weight)*bColour.hue() + (1-weight)*colour.hue();
            int s = (weight)*bColour.saturation() + (1-weight)*colour.saturation();
            int v = (weight)*bColour.value() + (1-weight)*colour.value();
            QColor c;
            c = c.fromHsv(h,s,v);
            if(base[p.x()+i + size*(p.y()+j)] != -1)
                image->setPixel(p.x()+i,p.y()+j,c.rgba());
       }
}

void LeafModel::drawFromVein(QImage* image, TreeNode* node, QColor colour, int intensity)
{
    QPoint p = node->position->toPoint();
    float intOrig  = intensity;
    QColor bColour;
    intensity = node->veinSize*intensity; //Scale by node

    float max = intensity;// sqrt(2*pow(intensity, 2)); //Max distance
    //alt colour full at p, base colour full outside p
    for(int i = -(intensity);i < (intensity); i++)
       for(int j = -(intensity);j < (intensity); j++)
       {
            bColour = QColor(image->pixel(p.x()+i,p.y()+j));
            float weight =  sqrt(pow(i, 2) + pow(j, 2))/max;

            if(weight > 1)
                weight = 1;

            int h = (weight)*bColour.hue() + (1-weight)*colour.hue();
            int s = (weight)*bColour.saturation() + (1-weight)*colour.saturation();
            int v = (weight)*bColour.value() + (1-weight)*colour.value();
            QColor c;
            c = c.fromHsv(h,s,v);
            if(base[p.x()+i + size*(p.y()+j)] != -1)
                image->setPixel(p.x()+i,p.y()+j,c.rgba());
       }
    //Now do children
    QListIterator<TreeNode*> nodeIt(*node->children);
    while(nodeIt.hasNext())
    {
        TreeNode* next = nodeIt.next();
        drawFromVein(image, next, colour, intOrig);
    }

}

void LeafModel::drawDebugGrid(QPointF *p)
{
    if(p == NULL)
        p = new QPointF(0,0);
    int x = p->toPoint().x()/gridSize;
    int y = p->toPoint().y()/gridSize;
    QImage image = QImage(gridCount,gridCount,QImage::Format_ARGB32);
    image.fill(QColor(255,255,255).rgba());


    for(int i=0; i< gridCount; i++)
    {

        for(int j=0; j< gridCount; j++)
        {
            if(x == i && y == j)
            {
                image.setPixel(x,y, QColor(0,0,0).rgba());
            }
            else
            {
                if(auxinGrid[i+j*gridCount]->size() > 0)
                    image.setPixel(i,j, QColor(0,255,0).rgba());
                else
                    image.setPixel(i,j, QColor(255,0,0).rgba());
            }
        }

        QListIterator<QPointF*> auxIt(auxin);
        while(auxIt.hasNext())
        {
            QPointF* p2 = auxIt.next();
            image.setPixel(p2->x()/gridSize,p2->y()/gridSize, QColor(0,0,255).rgba());
        }
        //qDebug()<< "Image Drawn";
        //image.save(QString("Debug.png"),"png");
       /* QImage image = QImage(size,size,QImage::Format_ARGB32);
        image.fill(QColor(255,255,255));


        for(int i=0; i< size; i++)
        {

            for(int j=0; j< size; j++)
            {
                if(x == i && y == j)
                {
                    image.setPixel(x,y, QColor(0,0,0).rgba());
                }
                else
                {
                    if(marginalBase[i+j*size] == 0)
                        image.setPixel(i,j, QColor(0,255,0).rgba());
                    else if (marginalBase[i+j*size] == -1)
                        image.setPixel(i,j, QColor(255,0,0).rgba());
                    else
                        image.setPixel(i,j, QColor(0,0,0).rgba());
                }
            }
            qDebug()<< "Image Drawn";
            image.save(QString("Debug.png"),"png");*/
    }
}

void LeafModel::drawFromSpeckled(QImage* image, QPointF* point, QColor colour, int intensity)
{
    QPoint p = point->toPoint();
    QColor bColour;

    float max = intensity;// sqrt(2*pow(intensity, 2)); //Max distance
    //alt colour full at p, base colour full outside p
    for(int i = -(intensity);i < (intensity); i++)
       for(int j = -(intensity);j < (intensity); j++)
       {
            bColour = bColour.fromRgba(image->pixel(point->x()+i, point->y()+j));
            float weight =  sqrt(pow(i, 2) + pow(j, 2))/max;
            if(weight > 1)
                weight = 1;
            int h = (weight)*bColour.hue() + (1-weight)*colour.hue();
            int s = (weight)*bColour.saturation() + (1-weight)*colour.saturation();
            int v = (weight)*bColour.value() + (1-weight)*colour.value();
            QColor c;
            c = c.fromHsv(h,s,v);

            if(base[p.x()+i + size*(p.y()+j)] != -1)
                image->setPixel(p.x()+i,p.y()+j,c.rgb());
       }
}

QImage* LeafModel::fromSpeckled(QImage* image, QColor altColor, int intensity)
{
    //Ten times auxin rate
    int numSpeckles = (int)area*spawnRate*pow(10,-5);

    for(int i =0; i < numSpeckles; i++)
    {
        //Now we throw darts
        float x = getRand(bounds[2],bounds[3]);
        float y = getRand(bounds[0],bounds[1]);

        //Our dart is at the very least inside the leaf
        while(base[(int)x+size*(int)y] == -1)
        {
            x = getRand(bounds[2],bounds[3]);
            y = getRand(bounds[0],bounds[1]);
        }
        QPointF* point = new QPointF(x,y);

        drawFromSpeckled(image, point, altColor, intensity);
    }
    return image;
}

QImage* LeafModel::drawVeinsOutline(QString name)
{
    QImage* veinImage = new QImage(size,size,QImage::Format_ARGB32);
    veinImage->fill(QColor(0,0,0,0).rgba());
    fillTexture(veinImage, QColor(255,255,255,255));
    QListIterator<QVector2D*> edgesIt(*edges);
    //First, draw in vector edges
    while(edgesIt.hasNext())
    {
        QVector2D* v = edgesIt.next();
        veinImage->setPixel(int(v->x()+root->x()),int(v->y()+root->y()), QColor(0,0,0,255).rgba());
    }


    QListIterator<TreeNode*> nodeIt(auxinTarget);
    QListIterator<QPointF*> auxIt(auxin);

    /*QPainter p;
    p.begin(veinImage);
    //qDebug()<<"- Calc Vectors";
    while (nodeIt.hasNext())
    {
        p.setPen(QPen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        TreeNode* node = nodeIt.next();
        QPointF* point = auxIt.next();
        p.drawLine(point->toPoint(),node->position->toPoint());
    }
    p.end();*/

    rootNode->draw(veinImage, QColor(0,0,0,255));


    veinImage->save(name, "png");
    return veinImage;
}

void LeafModel::createMaterialFile(QString name, QString path)
{
    QFile file(path+name+QString(".mtl"));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    out<< "newmtl "<<name<<"_top\n";

    out<< "Ka 0.0435 0.0435 0.0435\n";
    out<< "Kd "<<0.6<<"\n";
    out<< "Ks "<<spec<<"\n";
    out<< "illum 2\n";
    out<< "Ns "<<spec*spec*500<<"\n";
    out<< "\n";
    out<< "map_Ka -clamp on -s 1 1 1 -o 0 0 0 -mm 0 1 ./GeneratedLeaves/"<<name<<"_top.png"<<"\n";
    out<< "map_Kd -clamp on -s 1 1 1 -o 0 0 0 -mm 0 1 ./GeneratedLeaves/"<<name<<"_top.png"<<"\n";
    out<< "bump -clamp on -imfchan r -s 1 1 1 -o 0 0 0 -bm 1 ./GeneratedLeaves/"<<name<<"_topN.png"<<"\n";

    out<< "\n";
    out<< "newmtl "<<name<<"_bottom\n";

    out<< "Ka 0.0435 0.0435 0.0435\n";
    out<< "Kd "<<0.6<<"\n";
    out<< "Ks "<<spec/2<<"\n";
    out<< "illum 2\n";
    out<< "Ns "<<spec*spec*500<<"\n";
    out<< "\n";
    out<< "map_Ka -clamp on -s 1 1 1 -o 0 0 0 -mm 0 1 ./GeneratedLeaves/"<<name<<"_bottom.png"<<"\n";
    out<< "map_Kd -clamp on -s 1 1 1 -o 0 0 0 -mm 0 1 ./GeneratedLeaves/"<<name<<"_bottom.png"<<"\n";
    out<< "bump -clamp on -imfchan r -s 1 1 1 -o 0 0 0 -bm 1 ./GeneratedLeaves/"<<name<<"_bottomN.png"<<"\n";

    file.close();
}

void LeafModel::createBumpMaps()
{
    //Bump maps on red channel
    //first create bottom map, curving slightly inwards
    QColor depth = QColor(64,0,0,255);
    float max = (2/(float)128)*pow(256,2);
    depth.setRedF(0);
    topBumpMap->fill(QColor(0,0,0,0).rgba());
    topBumpMap = fillTexture(topBumpMap,depth);
    depth.setRedF(bump);
    bottomBumpMap->fill(QColor(0,0,0,0).rgba());
    bottomBumpMap = fillTexture(bottomBumpMap,depth);

    depth.setRedF(bump/3);
    fromSpeckled(topBumpMap, depth, 15);
    depth.setRedF(2*bump/3);
    fromSpeckled(bottomBumpMap, depth , 15);
    //set up basic curve

    for(int i = -size/2; i< size/2; i++)
        for(int j = -size/2; j< size/2; j++)
        {
            float vein = 0;
            if(topTexture->pixel(i+size/2,j+size/2) == rootColour->rgba())
            {
                    vein = bump;

                float val = (1/(float)128)*pow(i,2)+(1/(float)128)*pow(j,2);

                /*if(val/max > bump)
                    depth.setRedF(val/max - vein);
                else*/
                    depth.setRedF(vein);
                topBumpMap->setPixel(i+size/2,j+size/2,depth.rgb());

                /*if(val/max > bump)
                    depth.setRedF(1-val/max + vein);
                else*/
                    depth.setRedF(bump-vein);
                bottomBumpMap->setPixel(i+size/2,j+size/2,depth.rgb());
            }
        }
}
