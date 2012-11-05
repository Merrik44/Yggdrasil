#include "foliageparameters.h"
#include "foliagechooser.h"

#define radToDegree 57.2957795

FoliageParameters::FoliageParameters(QObject *parent) : QObject(parent)
{
    leaves = new QList<QString>();
    leaves->append(QString("Resources/Generated_Leaves/Leaf_Textures/default/default"));
    ratios = new QList<int>();
    ratios->append(1);
    numberLeavesMin = 1;
    numberLeavesMax = 1;
    spreadMin = 10;
    spreadMax = 20;
    scaleMin = 10;
    scaleMax = 10;

}

bool FoliageParameters::setValues()
{
    FoliageChooser* f = new FoliageChooser();
    connect(f, SIGNAL(returnValues(QList<QString>*, QList<int>*,
                                   int, int, float, float, float, float)),
           this, SLOT(valuesReturned(QList<QString>*,QList<int>*,
                                   int, int, float, float, float, float)));
    f->show();
}

void FoliageParameters::valuesReturned(QList<QString>* leaves, QList<int>* ratios,
                                       int numberLeavesMin, int numberLeavesMax,
                                       float spreadMin, float spreadMax,
                                       float scaleMin, float scaleMax)
{
    this->leaves = new QList<QString>(*leaves);
    this->ratios = new QList<int>(*ratios);
    this->numberLeavesMin = numberLeavesMin;
    this->numberLeavesMax = numberLeavesMax;
    this->spreadMin = spreadMin;
    this->spreadMax = spreadMax;
    this->scaleMin = scaleMin;
    this->scaleMax = scaleMax;

    emit foliageChanged();
}

string FoliageParameters::createMesh(QString lst)
{
    qsrand(QTime::currentTime().msec());
    this->lst = lst;


    if(parseLST(lst))
    {
        return createLeaves();
    }
    else
        return NULL;
}

bool FoliageParameters::parseLST(QString lst)
{
    string file = lst.toStdString();
    //Ok this code is just stolen from Richard, I'm goinng to trust that it works.
    ifstream myfile(file.c_str());

    if (myfile.is_open())
    {
        string command;

        vector<BranchNode*> branches;

        stack<BranchNode*> branchStack;
        stack<Vector3f> positionStack;
        stack<Matrix3f> rotationStack;

        BranchNode* current = NULL;
        Vector3f currentPosition;
        Matrix3f currentRotation;
        BranchNode* root = NULL;

        while (myfile.good())
        {
            getline (myfile,command);

            vector< string > tokens;
            // boost library function
            erase_all(command, " ");
            trim(command);
            split( tokens, command, is_any_of(" (),") );
            if( tokens.size() > 0 )
            {
                if (tokens[0] == "Cylinder")
                {
                    float startRadius = QString(tokens[1].c_str()).toFloat();
                    float endRadius = QString(tokens[2].c_str()).toFloat();
                    float length = QString(tokens[3].c_str()).toFloat();

                    current = new BranchNode( currentPosition, currentRotation, startRadius, endRadius, length, current);
                    branches.push_back(current);
                    if (root == NULL)
                        root = current;
                }
                else if (tokens[0] == "RotRel3f")
                {
                    float angle = -QString(tokens[1].c_str()).toFloat();
                    Vector3f axisAngle;
                    axisAngle.x = QString(tokens[2].c_str()).toFloat();
                    axisAngle.y = QString(tokens[3].c_str()).toFloat();
                    axisAngle.z = QString(tokens[4].c_str()).toFloat();
                    //qDebug()<<" "<<tokens[1].c_str()<<" <> ";
                    double newAngle = (angle*57.2957795);//angle*radToDegree;
                    axisAngle.x = (float)axisAngle.x*newAngle;
                    axisAngle.y = (float)axisAngle.y*newAngle;
                    axisAngle.z = (float)axisAngle.z*newAngle;
                    Matrix3f newRot;
                    newRot.identity();
                    newRot = newRot.createRotationAroundAxis(axisAngle.x, axisAngle.y, axisAngle.z);
                    //qDebug()<<"newRot ("<<axisAngle.x<<", "<<axisAngle.y<<", "<<axisAngle.z<<")\n";
                    currentRotation = currentRotation*newRot;

                    //qDebug()<<"Currnet rotation ("<<currentRotation.det()<<")\n";

                }
                else if (tokens[0] == "MovRel3f")
                {
                    Vector3f translation;
                    translation.x = QString(tokens[1].c_str()).toFloat();
                    translation.y = QString(tokens[2].c_str()).toFloat();
                    translation.z = QString(tokens[3].c_str()).toFloat();
                    translation = currentRotation*translation;
                    //qDebug()<<"translation ("<<translation.x<<", "<<translation.y<<", "<<translation.z<<")\n";
                    currentPosition +=  translation;

                }
                else if (tokens[0] == "SB")
                {
                    branchStack.push(current);
                    positionStack.push(currentPosition);
                    rotationStack.push(currentRotation);
                }
                else if (tokens[0] == "EB")
                {
                    current = branchStack.top();
                    branchStack.pop();
                    currentPosition = positionStack.top();
                    positionStack.pop();
                    currentRotation = rotationStack.top();
                    rotationStack.pop();

                }
            }
        }

        int maxHeight = branches[0]->endPosition.y;
        int minHeight = branches[0]->endPosition.y;

        endPoints.clear();
        // find the tips of all the branches
        for ( int i = 0; i < (int)branches.size(); i++ )
        {
            if(branches[i]->endPosition.y < minHeight)
            {
                minHeight = branches[i]->endPosition.y;
            }
            else if(branches[i]->endPosition.y > maxHeight)
            {
                maxHeight = branches[i]->endPosition.y;
            }

            if (  branches[i]->children.size() == 0)
                endPoints.append(branches[i]);

        }
        treeHeight = maxHeight - minHeight;

        myfile.close();
        return true;
    }
    else
        return false;

}

string FoliageParameters::createLeaves()
{
    string objName;
    QVector<QString> leafChoices;
    QVector<int> leafAttachment;
    QVector<LSTLeafNode*> nodes;


    QListIterator<QString> leafIt(*leaves);
    QListIterator<int> ratioIt(*ratios);
    while(leafIt.hasNext())
    {
        QString leaf = leafIt.next();
        int ratio = ratioIt.next();
        int attachment = findAttachment(leaf);
        for(int j=0; j < ratio; j++)
        {
            leafChoices.append(leaf);
            //find attachment point
            leafAttachment.append(attachment);
        }
    }

    int count = 0;
    //now we pick the random leaves
    for(int i=0; i< endPoints.count();i++)
    {
        int num = 1;//getIntRand(numberLeavesMin,numberLeavesMax);
        for(int j=0; j< num; j++)
        {
            count++;
            LSTLeafNode* leaf = new LSTLeafNode();
            //Get leaf material (according to ratio)
            int r = getIntRand(0,leafChoices.count()-1);
            QString name = QString(leafChoices[r]);
            name = name.mid(name.lastIndexOf("/")+1);
            leaf->name = name;
            leaf->number = count;
            leaf->scale = getRand(scaleMin,scaleMax);
            leaf->size = treeHeight*leaf->scale;
            leaf->scale = leaf->size/(float)512;
            leaf->attachment = leafAttachment[r];
            leaf->attachmentPoint = endPoints[i];
            leaf->rot = endPoints[i]->rotation;
            leaf->path = QString(leafChoices[r]);
            //leaf->path.chop(name.length()-leaf->path.lastIndexOf("/"));
            //leaf->path = QString("./")+ leaf->path.mid(leaf->path.indexOf("GeneratedLeaves"));

            float x = getRand(-0.1,0.1);
            //Rotate up-down and across
            float y = getRand(spreadMin,spreadMax);
            y -= (spreadMin + (spreadMax-spreadMin));
            float z = getRand(spreadMin,spreadMax);
            z -= (spreadMin + (spreadMax-spreadMin));

            //leaf->rot = leaf->rot.createRotationAroundAxis(x,y,z);
            /*leaf->p1 = Vector3f(-leaf->attachment*leaf->scale,0,0);
            leaf->p2 = Vector3f((512-leaf->attachment)*leaf->scale, 0,0);
            leaf->p3 = Vector3f(leaf->p2.x,0,leaf->size);
            leaf->p4 = Vector3f(leaf->p1.x,0,leaf->size);*/

            //Create upright quad
            leaf->attachment = 0.5;///= (float)512;

            leaf->p1 = Vector3f(-leaf->attachment,0,0);
            leaf->p2 = Vector3f(leaf->attachment, 0,0);
            leaf->p3 = Vector3f(leaf->attachment,1,0);
            leaf->p4 = Vector3f(-leaf->attachment,1,0);

            //Scale quad
            leaf->p1 *= leaf->scale;
            leaf->p2 *= leaf->scale;
            leaf->p3 *= leaf->scale;
            leaf->p4 *= leaf->scale;
            //Rotate within leaf cluster
            /*leaf->p1 = leaf->rot * leaf->p1;
            leaf->p2 = leaf->rot * leaf->p2;
            leaf->p3 = leaf->rot * leaf->p3;
            leaf->p4 = leaf->rot * leaf->p4;*/
            //Rotate on branch
            leaf->p1 = endPoints[i]->rotation * leaf->p1;
            leaf->p2 = endPoints[i]->rotation * leaf->p2;
            leaf->p3 = endPoints[i]->rotation * leaf->p3;
            leaf->p4 = endPoints[i]->rotation * leaf->p4;
            //Place at branch
            leaf->p1 += endPoints[i]->endPosition;
            leaf->p2 += endPoints[i]->endPosition;
            leaf->p3 += endPoints[i]->endPosition;
            leaf->p4 += endPoints[i]->endPosition;

            //so.. that's... something...
            nodes.append(leaf);
        }
    }

    //SO we should have just the lst name
    //ie john/files/tree3.lst gives us
    //tree3
    QString name = QString(lst);
    name.chop(name.length()-name.lastIndexOf("."));
    name = name.mid(name.lastIndexOf("/")+1);

    QString obj = QString("Resources/Generated_Leaves/Foliage_Models/")+name+QString("_foliage.obj");
    objName = obj.toStdString();
    QFile file(obj);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    count = 1;
    for(int i = 0; i< nodes.count();i++)
    {
        LSTLeafNode leaf = *nodes[i];
        out<<"g "<<leaf.name<<"\n";
        out<<"mtllib "<<leaf.path<<".mtl\n";
        out<<"usemtl "<< leaf.name<<"_top\n";
        out<<"v "<<leaf.p1.x<<" "<<leaf.p1.y<<" "<<leaf.p1.z<<"\n";
        out<<"v "<<leaf.p2.x<<" "<<leaf.p2.y<<" "<<leaf.p2.z<<"\n";
        out<<"v "<<leaf.p3.x<<" "<<leaf.p3.y<<" "<<leaf.p3.z<<"\n";
        out<<"v "<<leaf.p4.x<<" "<<leaf.p4.y<<" "<<leaf.p4.z<<"\n";
        out<<"vt 0 0\n";
        out<<"vt 0 1\n";
        out<<"vt 1 0\n";
        out<<"vt 1 1\n";
        out<<"f "<<count<<"/1 "<<count+1<<"/3 "<<count+2<<"/4 "<<count+3<<"/2\n";
        count+= 4;
        out<<"\n";
        //out<<"usemtl "<< leaf.tex<<"_bottom\n";
        //out<<"f "<<count-4<<"/2 "<<count-1<<"/1 "<<count-2<<"/3 "<<count-3<<"/4\n";//reversed order
        //out<<"\n";

    }

    file.close();
    if(file.exists(obj))
    {
        qDebug()<<obj;
        return objName;
    }
    else
        return NULL;
}

int FoliageParameters::findAttachment(QString mtl)
{
    QString text = QString(mtl);
    text.chop(text.length()-text.lastIndexOf("."));
    text.append("_top.png");
    QImage* f = new QImage(text);
    int min = -1;
    int max = 0;
    QColor back = f->pixel(0,0);

    for(int i = 0; i< f->width();i++)
    {
        //FInd first none clear pixel (this is the stem)
        if(QColor(f->pixel(i,f->height()-1)) != back && min == -1)
        {
            min = i;
        }
        //Find next clear pixel (end of stem)
        else if(QColor(f->pixel(i,f->height()-1)) == back && min != -1)
        {
            max = i-1;

            //return midpoint
            return min+ (max-min)/2;
        }
    }
    //if this doesn't work, just use the middle
    return f->width()/2;
}

int FoliageParameters::getIntRand(QPointF* p)
{
    int x = (int)(p->x()+0.5);
    int y = (int)(p->y()+0.5);
    return x + qrand()%(y+1-x);
}

int FoliageParameters::getIntRand(float x, float y)
{
    int px = (int)(x+0.5);
    int py = (int)(y+0.5);
    return px + qrand()%(py+1-px);
}

float FoliageParameters::getRand(QPointF* p)
{
    return p->x() + (float)qrand()/((float)RAND_MAX/(p->y()-p->x()));
}

float FoliageParameters::getRand(float x, float y)
{
    return x + (float)qrand()/((float)RAND_MAX/(y-x));
}

