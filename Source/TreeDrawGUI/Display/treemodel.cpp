#include "treemodel.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
#include "vector.h"
#include "matrix.h"
#include "point.h"

#include <TreeDrawGUI/Display/rapidxml-1.13/rapidxml.hpp>

using namespace rapidxml;
using namespace std;

const string FileHeader = "LST 0.1";

const int RENDER_MODE_LST = 1;
const int RENDER_MODE_XML = 2;


TreeModel::TreeModel()
{

}

TreeModel::~TreeModel()
{
    list<DisplayBranch*>::iterator e = renderList.end();
    for(list<DisplayBranch*>::iterator i = renderList.begin(); i != e; i++)
    {
        delete (*i);
    }

    list<Operation*>::iterator ex = XMLrenderList.end();
    for(list<Operation*>::iterator ix = XMLrenderList.begin(); ix != ex; ix++)
    {
        delete (*ix);
    }

}

TreeModel* TreeModel::createFromLST(const string &fileName)
{
    string s,operation;
    fstream source;
    list<DisplayBranch*> newRenderList;
    list<Operation*> newXmlList;
    size_t pos;
    char line[2048];

    source.open(fileName.c_str(),std::fstream::in);

    if (source)
    {
        getline(source, s);

        if (s == FileHeader)
        {
            float x,y,z,angle;
            float startRadius, endRadius, height;
            float segmentLength, lengthAlongSegment;

            stack<DisplayBranch*> branchStack;
            stack<float> segmentLengthStack, lengthAlongSegmentStack;
            DisplayBranch* currentBranch = new DisplayBranch();
            segmentLength = 1.0f;
            lengthAlongSegment = 0.0f;
            newRenderList.push_back(currentBranch);

            while (source.getline(line,2048) != 0)
            {
                s = std::string(line);
                //trim left space
                s.erase(s.begin(),std::find_if(s.begin(),s.end(),std::not1(std::ptr_fun<int, int>(std::isspace))));

                pos = s.find("(");

                if (pos == std::string::npos)
                    continue;

                operation = s.substr(0,pos);


                if (operation == "SB")
                {
                    branchStack.push(currentBranch);
                    segmentLengthStack.push(segmentLength);
                    lengthAlongSegmentStack.push(lengthAlongSegment);
                    currentBranch = new DisplayBranch(currentBranch, lengthAlongSegment/segmentLength);
                    newXmlList.push_back(new PushOperation());
                    newRenderList.push_back(currentBranch);
                }

                else if (operation == "EB")
                {
                    //currentBranch->constructCompleteBranch();

                    currentBranch = branchStack.top();
                    branchStack.pop();
                    segmentLength = segmentLengthStack.top();
                    segmentLengthStack.pop();
                    lengthAlongSegment = lengthAlongSegmentStack.top();
                    lengthAlongSegmentStack.pop();
                    newXmlList.push_back(new PopOperation());
                }

                else if (operation == "Cut")
                    //newRenderList.push_back(new CutOperation());
                    x = 0.0f;

                else if (operation == "MovRel3f")
                {
                    sscanf(s.substr(pos).c_str(),"(%f, %f, %f)", &x, &y, &z);
                    newXmlList.push_back(new TranslateOperation(x, y, z));
                    currentBranch->applyTranslation(x, y, z);
                    lengthAlongSegment += y;
                }

                else if (operation == "RotRel3f")
                {
                    sscanf(s.substr(pos).c_str(),"(%f, %f, %f, %f)", &angle, &x, &y, &z);
                    newXmlList.push_back(new RotateOperation(angle, x, y, z));
                    currentBranch->applyRotation(angle, x, y, z);
                }

                else if (operation == "Cylinder")
                {
                    sscanf(s.substr(pos).c_str(),"(%f, %f, %f)", &startRadius, &endRadius, &height);
                    newXmlList.push_back(new CylinderOperation(startRadius, endRadius, height));
                    currentBranch->addBranch(startRadius, endRadius, height);
                    segmentLength = height;
                    lengthAlongSegment = 0.0f;
                }

            }

            //currentBranch->constructCompleteBranch();
            list<DisplayBranch*>::iterator e = newRenderList.end();
            for(list<DisplayBranch*>::iterator i = newRenderList.begin(); i != e; i++)
            {
                (*i)->constructCompleteBranch();
            }

            if (source.eof())
            {
                TreeModel *output = new TreeModel();
                output->renderList = newRenderList;
                //output->XMLrenderList = newXmlList;
                output->renderMode = RENDER_MODE_LST;

                return output;
            }
        }

        source.close();

    }
    return 0;
}


pair<float,float> extractParameterRange(const xml_node<> *node)
{
    float first,second;
    sscanf(node->first_node("min",0,false)->value(),"%f",&first);
    sscanf(node->first_node("max",0,false)->value(),"%f",&second);
        return make_pair(first,second);
}

void readBranch(list<Operation*> *opList, const xml_node<> *node)
{
    pair<float,float> lengthP;
    lengthP = extractParameterRange(node->first_node("length", 0, false));
    float length = (lengthP.first + lengthP.second)/20;
    pair<float,float> sr;
    sr = extractParameterRange(node->first_node("thickness", 0, false)->first_node("start", 0, false));
    pair<float,float> er;
    er = extractParameterRange(node->first_node("thickness", 0, false)->first_node("end", 0, false));
    float startr = (sr.first + sr.second)/20;
    float endr = (er.first + er.second)/20;


    opList->push_back(new CylinderOperation(startr,endr,length));

    xml_node<> *children = node->first_node("children", 0, false);
    for (xml_node<> *child = children->first_node("branch"); child; child = child->next_sibling("branch"))
    {
        float relativePosition;
        sscanf(child->first_node("positionOnParent", 0, false)->value(), "%f", &relativePosition);

        xml_node<> *angleToParentNode = child->first_node("angleToParent", 0, false);
        pair<float,float> angleP = extractParameterRange(angleToParentNode->first_node("inclination", 0, false));
        pair<float,float> rotational = extractParameterRange(angleToParentNode->first_node("azimuth", 0, false));
        float angle = (angleP.first + angleP.second)/2;
        float aroundAngle = rotational.first;

        opList->push_back(new PushOperation());

        opList->push_back(new TranslateOperation(0.0f,length*relativePosition,0.0f));

        opList->push_back(new RotateOperation(aroundAngle,0.0f,1.0f,0.0f));

        opList->push_back(new RotateOperation(angle,0.0f,0.0f,1.0f));

        readBranch(opList,child);

        opList->push_back(new PopOperation());
    }

}


TreeModel* TreeModel::createFromXML(const string &fileName)
{
    ifstream fin(fileName.c_str(),ios::binary);

    string line;
    string *data = new string();
    while (getline(fin, line))
        *data += line + '\n';


    char* xmlData = new char[data->size()+1];
    memcpy(xmlData,data->c_str(),data->size()+1);

    delete data;

    fin.close();

    xml_document<> *document = new xml_document<>();
    document->parse<0>(xmlData);

    list<Operation*> newRenderList;

    readBranch(&newRenderList, document->first_node("branch",0,false));


    TreeModel *output = new TreeModel();
    output->XMLrenderList = newRenderList;
    output->renderMode = RENDER_MODE_XML;

    return output;
}


void TreeModel::Render()
{
    if (renderMode == RENDER_MODE_LST)
    {
        list<DisplayBranch*>::iterator e = renderList.end();
        for(list<DisplayBranch*>::iterator i = renderList.begin(); i != e; i++)
        {
            (*i)->Render();
        }

    }
    else if (renderMode == RENDER_MODE_XML);
    {

        list<Operation*>::iterator e = XMLrenderList.end();
        Matrix m = Matrix::identityMatrix();
        stack<Matrix> s;
        glTexCoord2d(0.1f,0.1f);
        for(list<Operation*>::iterator i = XMLrenderList.begin(); i != e; i++)
        {
            int t = (*i)->Render(m);
            if (t == 1)
            {
                s.push(Matrix(m));
            }
            if (t == 2)
            {
                m = s.top();
                s.pop();
            }
        }
    }
}


void TreeModel::calculateCameraSettings(float &x, float &y, float &z, float &ox, float &oy, float &oz)
{
    if (renderMode == RENDER_MODE_LST)
    {
        x = 0.0f; y = 0.0f; z = 0.0f;
        int count = 0;

        list<DisplayBranch*>::iterator e = renderList.end();
        for(list<DisplayBranch*>::iterator i = renderList.begin(); i != e; i++)
        {
            (*i)->modifyGravity(x,y,z,count);
        }
        x /= count;
        y /= count;
        z /= count;

        ox = 0.0f; oy = 0.0f; oz = 0.0f;
        for(list<DisplayBranch*>::iterator i = renderList.begin(); i != e; i++)
        {
            (*i)->findBounds(x,y,z,ox,oy,oz);
        }
    }
    else
    {
        x = 0; y = 0; z = 0;
        ox = 100; oy = 100; oz = 100;
    }
}


void TreeModel::writeToObj(const std::string &fileName)
{
    if (renderMode == RENDER_MODE_LST)
    {
        /*char* mtlname = strdup(fileName.c_str());
        int l = strlen(mtlname);
        mtlname[l-3] = 'm';
        mtlname[l-2] = 't';
        mtlname[l-1] = 'l';
        ofstream fout2(mtlname);

        fout2 << "newmtl wood" << endl;
        fout2 << "map_Ka woodtexture.raw" << endl;
        fout2.close();*/


        ofstream fout(fileName.c_str());

        //fout << "mtllib test.mtl" << endl;
        //fout << "usemtl wood" << endl;
        if (fout == 0)
        {
            //error
        }
        else
        {
            list<DisplayBranch*>::iterator e = renderList.end();
            for(list<DisplayBranch*>::iterator i = renderList.begin(); i != e; i++)
            {
                (*i)->writeToObj(fout);
            }
        }
    }
}
