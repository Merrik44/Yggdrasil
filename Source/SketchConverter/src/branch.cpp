/*
 * branch.cpp
 *
 *  Created on: Aug 23, 2011
 *      Author: Mark
 */

#include "branch.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <queue>
#include <algorithm>
#include <string.h>

using namespace rapidxml;
using namespace std;

const float BRANCH_CONTINUATION_THRESHOLD = 0.95f;
const float BRANCH_CONTINUATION_RADIUS_MATCH = 0.2f;
const float BRANCH_CLUSTER_RANGE = 0.05f;
const float BRANCH_JITTER_AMOUNT = 0.1f;
const int VARIANCE_FACTOR = 3;
const char* FORMAT_SPECIFIER = "%.6f";
const int DISTANCE_FIELD_SIZE = 64;
const int DISTANCE_FIELD_HALF_SIZE= 32;
const int DISTANCE_FIELD_VERTICAL_SIZE = 64;
const int DISTANCE_FIELD_EFFECT_AREA = 32;
const float INF = 9.0e20;
const float SIGMA = 1e-1;


Branch::Branch()
{
    rotationalAngle = 0.0f;
    parent = 0;
}

Branch::~Branch()
{
    //dtor
}

void Branch::clean()
{
}

Branch::Branch(const Branch& other)
{
    angleRange = pair<float,float>(other.angleRange);
    startThicknessRange = pair<float,float>(other.startThicknessRange);
    endThicknessRange = pair<float,float>(other.endThicknessRange);
    lengthRange = pair<float,float>(other.lengthRange);
    rotationalAngle = other.rotationalAngle;
    averageLength = other.averageLength;
    averageAngle = other.averageAngle;
    isSubSegment = other.isSubSegment;

    relativePosition = other.relativePosition;

    parent = other.parent;
    //if (parent != 0)
    	//parent->childBranches.push_back(this);

    for (int i=other.childBranches.size()-1; i>=0; --i)
    {
        childBranches.push_back(new Branch(*other.childBranches[i]));
    }

}


void Branch::flipBranch()
{
	swap(angleRange.first, angleRange.second);
	angleRange.first = -angleRange.first;
	angleRange.second = -angleRange.second;
    averageAngle = -averageAngle;


	for (int i = childBranches.size()-1; i>=0; --i)
	{
		childBranches[i]->flipBranch();
	}

}

pair<float,float> extractParameterRange(const xml_node<> *node)
{
    float first,second;
    sscanf(node->first_node("min",0,false)->value(),"%f",&first);
    sscanf(node->first_node("max",0,false)->value(),"%f",&second);
	return make_pair(first,second);
}

void insertParameterRange(xml_document<> *document, xml_node<> *node, pair<float,float> range)
{
    char output[64];
    sprintf(output,FORMAT_SPECIFIER,range.first);
    char *minValue = document->allocate_string(output);
    xml_node<> *min = document->allocate_node(node_element,"min",minValue);
    sprintf(output,FORMAT_SPECIFIER,range.second);
    char *maxValue = document->allocate_string(output);
    xml_node<> *max = document->allocate_node(node_element,"max",maxValue);
    node->append_node(min);
    node->append_node(max);
}



Branch* Branch::constructTreeFromXML(const xml_document<> *document)
{
    return Branch::constructBranchFromXML(document->first_node("branch",0,false),0);
}

Branch* Branch::constructBranchFromXML(const xml_node<> *node, Branch* parent)
{
    Branch* branch = new Branch();

    branch->parent = parent;

    sscanf(node->first_node("positionOnParent", 0, false)->value(), "%f", &branch->relativePosition);


    /*branch->angleRange = extractParameterRange(node->first_node("angleToParent", 0, false));
    branch->rotationalAngle = 0.0f;
    if (branch->angleRange.first > M_PI)
    {
    	float temp = branch->angleRange.first;
    	branch->angleRange.first = 2 * M_PI - branch->angleRange.second;
    	branch->angleRange.second = 2 * M_PI - temp;
    	branch->rotationalAngle = 180.0f;
    }*/

    xml_node<> *angleToParentNode = node->first_node("angleToParent", 0, false);
    branch->angleRange = extractParameterRange(angleToParentNode->first_node("inclination", 0, false));
    pair<float,float> rotational = extractParameterRange(angleToParentNode->first_node("azimuth", 0, false));
    branch->rotationalAngle = rotational.first * 180.0f / M_PI;

    branch->averageAngle = (branch->angleRange.first + branch->angleRange.second)/2;

    if (node->first_node("isSubSegment", 0, false))
    {
    	branch->isSubSegment = !strcmp(node->first_node("isSubSegment", 0, false)->value(), "true");
    }
    else
    {
    	branch->isSubSegment = false;
    }

    /*if (branch->angleRange.first > M_PI)
    {
    	branch->angleRange.first = 2 * M_PI - branch->angleRange.first;
    	branch->angleRange.second = 2 * M_PI - branch->angleRange.second;
    	branch->rotationalAngle = 180.0f;
    }*/


    branch->startThicknessRange = extractParameterRange(node->first_node("thickness", 0, false)
                                                           ->first_node("start", 0, false));
    branch->averageStartThickness = (branch->startThicknessRange.first +  branch->startThicknessRange.second) / 2.0f;

    branch->endThicknessRange = extractParameterRange(node->first_node("thickness", 0, false)
                                                         ->first_node("end", 0, false));
    branch->averageEndThickness = (branch->endThicknessRange.first +  branch->endThicknessRange.second) / 2.0f;

    branch->lengthRange = extractParameterRange(node->first_node("length", 0, false));
    branch->averageLength = (branch->lengthRange.first + branch->lengthRange.second)/2;


    xml_node<> *children = node->first_node("children", 0, false);
    for (xml_node<> *child = children->first_node("branch"); child; child = child->next_sibling("branch"))
    {
		branch->childBranches.push_back(Branch::constructBranchFromXML(child,branch));
	}

    if (branch->angleRange.second < 0.0f)
    {
    	/*float temp = branch->angleRange.first;
    	branch->angleRange.first = -branch->angleRange.second;
    	branch->angleRange.second = -temp;*/
    	branch->flipBranch();
        branch->rotationalAngle += 180.0f;
    }



	return branch;
}

void Branch::writeTreeToXMLDocument(xml_document<> *document)
{
    xml_node<> *branch = document->allocate_node(node_element,"branch");
    writeBranchToXMLDocument(document, branch);
    document->append_node(branch);
}

void Branch::writeBranchToXMLDocument(xml_document<> *document, xml_node<> *branch)
{
    char output[256];



    xml_node<> *curveNode = document->allocate_node(node_element,"curve");
    branch->append_node(curveNode);


    sprintf(output,FORMAT_SPECIFIER,relativePosition);
    char *position = document->allocate_string(output);
    xml_node<> *relativePositionNode = document->allocate_node(node_element,"positionOnParent",position);
    branch->append_node(relativePositionNode);


    xml_node<> *angletoParentNode = document->allocate_node(node_element,"angleToParent");
    branch->append_node(angletoParentNode);

    if (rotationalAngle >= 180.0f)
        {
        	rotationalAngle -= 180.0f;
        	/*float temp = angleRange.first;
        	angleRange.first = -angleRange.second;
        	angleRange.second = -temp;*/
        	flipBranch();
        }
    if (rotationalAngle < 0.0f)
	{
		rotationalAngle += 180.0f;
		/*float temp = angleRange.first;
		angleRange.first = -angleRange.second;
		angleRange.second = -temp;*/
		flipBranch();
	}


    xml_node<> *inclinationNode = document->allocate_node(node_element,"inclination");
	insertParameterRange(document, inclinationNode, angleRange);
	angletoParentNode->append_node(inclinationNode);

	xml_node<> *azimuthNode = document->allocate_node(node_element,"azimuth");
	insertParameterRange(document, azimuthNode, make_pair(rotationalAngle/180 * M_PI, rotationalAngle/180 * M_PI));
	angletoParentNode->append_node(azimuthNode);



    xml_node<> *thicknessNode = document->allocate_node(node_element,"thickness");
    xml_node<> *start = document->allocate_node(node_element,"start");
    xml_node<> *end = document->allocate_node(node_element,"end");
    insertParameterRange(document, start,startThicknessRange);
    insertParameterRange(document, end,endThicknessRange);
    thicknessNode->append_node(start);
    thicknessNode->append_node(end);
    branch->append_node(thicknessNode);


    xml_node<> *lengthNode = document->allocate_node(node_element,"length");
    insertParameterRange(document, lengthNode,lengthRange);
    branch->append_node(lengthNode);

    char *subsegment;
    if (isSubSegment)
        subsegment = document->allocate_string("true");
    else
        subsegment = document->allocate_string("false");
    xml_node<> *isSubSegmentNode = document->allocate_node(node_element,"isSubSegment", subsegment);
    branch->append_node(isSubSegmentNode);


    xml_node<> *childrenNode = document->allocate_node(node_element,"children");
    branch->append_node(childrenNode);
    xml_node<> *childNode;

    for (int i=0; i<childBranches.size(); ++i)
    {
        childNode = document->allocate_node(node_element,"branch");
        childBranches[i]->writeBranchToXMLDocument(document,childNode);
        childrenNode->append_node(childNode);
    }

}




void Branch::convertTo3D(Branch* root)
{
    srand((unsigned int)(root->averageLength * 1e8f));
    root->complexRotationAroundTrunk();
    root->simpleRotationAroundBranches(true);
}


void Branch::preprocessBranches()
{
	if (parent != 0)
	{

	}
}

void Branch::simpleRotationAroundBranches(bool isTrunk)
{
    if (isTrunk && false)
    {
        for (int i=0; i<childBranches.size(); ++i)
        {
            //childBranches[i]->simpleRotationAroundBranches(isTrunk && childBranches[i]->relativePosition > BRANCH_CONTINUATION_THRESHOLD);
        	childBranches[i]->simpleRotationAroundBranches(false);
        }
    }

    else
    {
        Branch* current = 0;
        Branch* continuation = this;
        vector<Branch*> extendedBranchChildren;
        vector<Branch*> continuationCandidatesNoRadius, continuationCandidatesRadius;

        do
        {
            current = continuation;
            continuation = 0;
            continuationCandidatesNoRadius.clear();
            continuationCandidatesRadius.clear();

            for (int i=0; i<current->childBranches.size(); ++i)
			{

            	if (current->childBranches[i]->isSubSegment)
            	{
            		continuation = current->childBranches[i];
            	}
            	else if (current->childBranches[i]->relativePosition >= BRANCH_CONTINUATION_THRESHOLD)
				{
					continuationCandidatesNoRadius.push_back(current->childBranches[i]);
					if ( fabs( (current->childBranches[i]->averageStartThickness - current->averageEndThickness) / current->averageEndThickness) < BRANCH_CONTINUATION_RADIUS_MATCH )
					{
						continuationCandidatesRadius.push_back(current->childBranches[i]);
					}
				}
				else
				{
                    extendedBranchChildren.push_back(current->childBranches[i]);
				}
			}

            if (continuation != 0)
            {
            	for (int i=0; i<continuationCandidatesNoRadius.size(); ++i)
				{
					extendedBranchChildren.push_back(continuationCandidatesNoRadius[i]);
				}
            }
            else if (continuationCandidatesNoRadius.size() == 1)
            {
            	continuation = continuationCandidatesNoRadius.front();
            }
            else if (continuationCandidatesRadius.size() == 1)
            {
            	continuation = continuationCandidatesRadius.front();
            	for (int i=0; i<continuationCandidatesNoRadius.size(); ++i)
            	{
                    if (continuationCandidatesNoRadius[i] != continuation)
                    {
                    	extendedBranchChildren.push_back(continuationCandidatesNoRadius[i]);
                    }
            	}
            }
            else if (continuationCandidatesNoRadius.size() > 0)
            {
            	if (isTrunk)
            	{
					for (int i=0; i<continuationCandidatesNoRadius.size(); ++i)
					{
						continuationCandidatesNoRadius[i]->simpleRotationAroundBranches(true);
					}
            	}
            	else
            	{
					for (int i=0; i<continuationCandidatesNoRadius.size(); ++i)
					{
						extendedBranchChildren.push_back(continuationCandidatesNoRadius[i]);
					}
            	}
            }


        }   while (continuation != 0);


        if (!isTrunk)
        {
			random_shuffle(extendedBranchChildren.begin(),extendedBranchChildren.end());
			int distanceField[360];
			for (int i=0;i<360;++i)
				distanceField[i] = 359;

			for (int i=0; i<extendedBranchChildren.size(); ++i)
			{
				int position;
				if (i == 0)
				{
					position = rand() % 360;
				}
				else
				{
					position = 0;
					for (int j=1;j<360;++j)
					{
						if (distanceField[j] > distanceField[position])
							position = j;
					}
				}
				int variance = distanceField[position] / VARIANCE_FACTOR;

				//currently it equally likely, should probably be a normal distribution
				int d = rand() % (2*variance) - variance;
				position += d;

				position %= 360;
				if (position < 0) position += 360;

				distanceField[position] = 0;

				for (int j=position-1;;--j)
				{
					if (j < 0)
						j += 360;

					if (j == 359)
					{
						if (distanceField[359] > distanceField[0] + 1)
						{
							distanceField[359] = distanceField[0] + 1;
						}
						else
						{
							break;
						}
					}
					else
					{
						if (distanceField[j] > distanceField[j+1] + 1)
						{
							distanceField[j] = distanceField[j+1] + 1;
						}
						else
						{
							break;
						}
					}
				}

				for (int j=position+1;;++j)
				{
					if (j > 359)
						j -= 360;

					if (j == 0)
					{
						if (distanceField[0] > distanceField[359] + 1)
						{
							distanceField[0] = distanceField[359] + 1;
						}
						else
						{
							break;
						}
					}
					else
					{
						if (distanceField[j] > distanceField[j-1] + 1)
						{
							distanceField[j] = distanceField[j-1] + 1;
						}
						else
						{
							break;
						}
					}
				}

				extendedBranchChildren[i]->rotationalAngle = position;
			}
        }

        for (int i=0; i<extendedBranchChildren.size(); ++i)
        {
            extendedBranchChildren[i]->simpleRotationAroundBranches(false);
        }

    }

}

struct point
{
	point(){}
	point(int ax, int ay, int az) : x(ax), y(ay), z(az)
	{

	}

	int x,y,z;
};

bool approxEqual(const float &a, const float &b)
{
	float d = a-b;
	return d < SIGMA && d > -SIGMA;
}

void insertPoint(float*** distanceField, int i, int j, int k)
{
	queue<point> q;
	q.push(point(i,j,k));

	int dx,dy,dz;
	point t;
	float d;
	while (!q.empty())
	{
		t = q.front();
		q.pop();
		dx = i-t.x;
		dy = j-t.y;
		dz = k-t.z;
		d = sqrtf(dx*dx + dy*dy + dz*dz);
		if (distanceField[t.x][t.y][t.z] > d && (dx < DISTANCE_FIELD_EFFECT_AREA && dy < DISTANCE_FIELD_EFFECT_AREA && dz < DISTANCE_FIELD_EFFECT_AREA))
		{
			distanceField[t.x][t.y][t.z] = d;

			if (t.x > 0)
							q.push(point(t.x-1,t.y,t.z));
			if (t.x < DISTANCE_FIELD_SIZE)
							q.push(point(t.x+1,t.y,t.z));
			if (t.y > 0)
							q.push(point(t.x,t.y-1,t.z));
			if (t.y < DISTANCE_FIELD_SIZE)
							q.push(point(t.x,t.y+1,t.z));
			if (t.z > 0)
							q.push(point(t.x,t.y,t.z-1));
			if (t.z < DISTANCE_FIELD_VERTICAL_SIZE)
							q.push(point(t.x,t.y,t.z+1));

		}
	}
}

void insertAtRandomPosition(Branch* branch, float*** distanceField, float maxHeight, float maxWidth)
{
	int radius = (int)((sin(branch->averageAngle)*branch->averageLength) / maxWidth  * DISTANCE_FIELD_HALF_SIZE);
	int height = max(0, (int)((branch->parent->trunkHeight + cos(branch->averageAngle)*branch->averageLength) / maxHeight * DISTANCE_FIELD_VERTICAL_SIZE));

	float offset = ((float)rand()) / RAND_MAX * M_PI / 2   -  M_PI/4;
	float lengthenFactor = fabs(cosf(offset));
	branch->rotationalAngle += offset / M_PI * 180.0f;
	//branch->lengthRange.first /= lengthenFactor;
	//branch->lengthRange.second /= lengthenFactor;

	int dx = (int)(cos(branch->rotationalAngle / 180.0f * M_PI) * radius);
	int dy = (int)(sin(branch->rotationalAngle / 180.0f * M_PI) * radius);

	insertPoint(distanceField, DISTANCE_FIELD_HALF_SIZE + dx, DISTANCE_FIELD_HALF_SIZE + dy, height);
	insertPoint(distanceField, DISTANCE_FIELD_HALF_SIZE + dx/2, DISTANCE_FIELD_HALF_SIZE + dy/2, height);

}

void insertAtBestPosition(Branch* branch, float*** distanceField, float maxHeight, float maxWidth, bool anywhere)
{
	int radius = (int)((sin(branch->averageAngle)*branch->averageLength) / maxWidth  * DISTANCE_FIELD_HALF_SIZE);
	int height = max(0, (int)((branch->parent->trunkHeight + cos(branch->averageAngle)*branch->averageLength) / maxHeight * DISTANCE_FIELD_VERTICAL_SIZE));

	int d = 3 - (2*radius);
	int x = 0;
	int y = radius;

	int maxX,maxY;
	int my;
	int undefCount = 1;
	float maxV;

	if (anywhere)
        {
            maxV = 0.0f;

            while (y >= x)
            {
                for (int i=-1; i<=1; i += 2)
                {
                    for (int j=-1; j<=1; j += 2)
                    {
                        if (maxV == INF)
                        {
                                if (distanceField[DISTANCE_FIELD_HALF_SIZE + i * y][DISTANCE_FIELD_HALF_SIZE + j * x][height] == INF)
                                {
                                    ++undefCount;
                                    if ((float)(rand())/RAND_MAX * (undefCount) < 1.0f)
                                    {
                                        maxX = i * y;
                                        maxY = j * x;
                                    }
                                }
                                if (distanceField[DISTANCE_FIELD_HALF_SIZE + i * x][DISTANCE_FIELD_HALF_SIZE + j * y][height] == INF)
                                {
                                    ++undefCount;
                                    if ((float)(rand())/RAND_MAX * (undefCount) < 1.0f)
                                    {
                                        maxX = i * x;
                                        maxY = j * y;
                                    }
                                }
                        }
                        else
                        {
                                if (distanceField[DISTANCE_FIELD_HALF_SIZE + i * y][DISTANCE_FIELD_HALF_SIZE + j * x][height] > maxV)
                                {
                                    maxX = i * y;
                                    maxY = j * x;
                                    maxV = distanceField[DISTANCE_FIELD_HALF_SIZE + i * y][DISTANCE_FIELD_HALF_SIZE + j * x][height];
                                }
                                if (distanceField[DISTANCE_FIELD_HALF_SIZE + i * x][DISTANCE_FIELD_HALF_SIZE + j * y][height] > maxV)
                                {
                                    maxX = i * x;
                                    maxY = j * y;
                                    maxV = distanceField[DISTANCE_FIELD_HALF_SIZE + i * x][DISTANCE_FIELD_HALF_SIZE + j * y][height];
                                }
                        }
                    }
                }

                if (d < 0)
                        d += 4*x++ + 6;
                else
                        d += 4*(x++ - y--) + 10;
            }
	}

	else if (approxEqual(branch->rotationalAngle,0.0f) || approxEqual(branch->rotationalAngle,180.0f))
	{
		if (approxEqual(branch->rotationalAngle,0.0f))
			my = 1;
		else
			my = -1;
		maxV = 0.0f;

		while (y >= x)
		{
			if (maxV == INF)
			{
                                if (distanceField[DISTANCE_FIELD_HALF_SIZE + my * y][DISTANCE_FIELD_HALF_SIZE + x][height] == INF)
				{
                                    ++undefCount;
                                    if ((float)(rand())/RAND_MAX * (undefCount) < 1.0f)
                                    {
					maxX = my*y;
                                        maxY = x;
                                    }
				}
                                if (distanceField[DISTANCE_FIELD_HALF_SIZE + my * y][DISTANCE_FIELD_HALF_SIZE - x][height] == INF)
				{
                                    ++undefCount;
                                    if ((float)(rand())/RAND_MAX * (undefCount) < 1.0f)
                                    {
					maxX = my*y;
                                        maxY = -x;
                                    }
				}
			}
			else
			{
				if (distanceField[DISTANCE_FIELD_HALF_SIZE + my * y][DISTANCE_FIELD_HALF_SIZE + x][height] > maxV)
				{
					maxX = my*y;
					maxY = x;
					maxV = distanceField[DISTANCE_FIELD_HALF_SIZE + my * y][DISTANCE_FIELD_HALF_SIZE + x][height];
				}
				if (distanceField[DISTANCE_FIELD_HALF_SIZE + my * y][DISTANCE_FIELD_HALF_SIZE - x][height] > maxV)
				{
					maxX = my*y;
					maxY = -x;
					maxV = distanceField[DISTANCE_FIELD_HALF_SIZE + my * y][DISTANCE_FIELD_HALF_SIZE - x][height];
				}
			}

			if (d < 0)
				d += 4*x++ + 6;
			else
				d += 4*(x++ - y--) + 10;
		}
	}

	else
	{
		if (approxEqual(branch->rotationalAngle,90.0f))
			my = 1;
		else
			my = -1;
		maxV = 0.0f;

		while (y >= x)
		{
			if (maxV == INF)
			{
                                if (distanceField[DISTANCE_FIELD_HALF_SIZE + x][DISTANCE_FIELD_HALF_SIZE + my * y][height] == INF)
				{
                                    ++undefCount;
                                    if ((float)(rand())/RAND_MAX * (undefCount) < 1.0f)
                                    {
					maxX = x;
                                        maxY = my * y;
                                    }
				}
                                if (distanceField[DISTANCE_FIELD_HALF_SIZE - x][DISTANCE_FIELD_HALF_SIZE + my * y][height] == INF)
				{
                                    ++undefCount;
                                    if ((float)(rand())/RAND_MAX * (undefCount) < 1.0f)
                                    {
					maxX = -x;
                                        maxY = my * y;
                                    }
				}
			}
			else
			{
				if (distanceField[DISTANCE_FIELD_HALF_SIZE + x][DISTANCE_FIELD_HALF_SIZE + my * y][height] > maxV)
				{
					maxX = x;
					maxY = my * y;
					maxV = distanceField[DISTANCE_FIELD_HALF_SIZE + x][DISTANCE_FIELD_HALF_SIZE + my * y][height];
				}
				if (distanceField[DISTANCE_FIELD_HALF_SIZE - x][DISTANCE_FIELD_HALF_SIZE + my * y][height] > maxV)
				{
					maxX = -x;
					maxY = my * y;
					maxV = distanceField[DISTANCE_FIELD_HALF_SIZE - x][DISTANCE_FIELD_HALF_SIZE + my * y][height];
				}
			}

			if (d < 0)
				d += 4*x++ + 6;
			else
				d += 4*(x++ - y--) + 10;
		}
	}

	if (maxV > 0.0f)
	{
		insertPoint(distanceField, DISTANCE_FIELD_HALF_SIZE + maxX, DISTANCE_FIELD_HALF_SIZE + maxY, height);
		insertPoint(distanceField, DISTANCE_FIELD_HALF_SIZE + maxX/2, DISTANCE_FIELD_HALF_SIZE + maxY/2, height);
		float angle = atan2(maxY,maxX);
		float lengthenFactor = fabs(cosf((branch->rotationalAngle / 180.0f * M_PI) - angle));
		//branch->lengthRange.first /= lengthenFactor;
		//branch->lengthRange.second /= lengthenFactor;
		branch->rotationalAngle = angle / M_PI * 180.0f;
	}
	else
	{
		insertAtRandomPosition(branch, distanceField, maxHeight, maxWidth);
	}

}

bool branchOrder(const Branch *a, const Branch *b)
{
	return a->relativePosition < b->relativePosition;
}

void Branch::complexRotationAroundTrunk()
{
	Branch* current = 0;
	Branch* continuation = this;
	bool splits = false;
	vector<Branch*> eastChildren, westChildren, northChildren, southChildren;
	vector< pair<float, Branch*> > copyChildren;

	float maxHeight = 0.0f;
	float maxWidth = 0.0f;
	float trunkLength = 0.0f;
	bool addLater = false;

	vector<Branch*> continuationCandidatesNoRadius, continuationCandidatesRadius;
	vector<Branch*> trunk;

	do
	{
		current = continuation;
		current->trunkHeight = trunkLength;
		trunk.push_back(current);
		trunkLength += current->averageLength;
		continuation = 0;
		continuationCandidatesNoRadius.clear();
		continuationCandidatesRadius.clear();

		for (int i=0; i<current->childBranches.size(); ++i)
		{

			if (current->childBranches[i]->isSubSegment)
			{
				continuation = current->childBranches[i];
			}
			else if (current->childBranches[i]->relativePosition >= BRANCH_CONTINUATION_THRESHOLD)
			{
				continuationCandidatesNoRadius.push_back(current->childBranches[i]);
				if ( fabs( (current->childBranches[i]->averageStartThickness - current->averageEndThickness) / current->averageEndThickness) < BRANCH_CONTINUATION_RADIUS_MATCH )
				{
					continuationCandidatesRadius.push_back(current->childBranches[i]);
				}
			}
			else
			{
				copyChildren.push_back(make_pair(current->trunkHeight + current->averageLength * current->childBranches[i]->relativePosition, current->childBranches[i]));
				maxHeight = max(maxHeight, (float)(cos(current->childBranches[i]->angleRange.first) * current->childBranches[i]->averageLength));
				maxWidth = fmax(maxWidth, fabs((float)(sin(current->childBranches[i]->averageAngle) * current->childBranches[i]->averageLength)));
			}
		}

		if (continuation != 0)
		{
			for (int i=0; i<continuationCandidatesNoRadius.size(); ++i)
			{
				copyChildren.push_back(make_pair(current->trunkHeight + current->averageLength * continuationCandidatesNoRadius[i]->relativePosition, continuationCandidatesNoRadius[i]));
				maxHeight = max(maxHeight, (float)(cos(continuationCandidatesNoRadius[i]->angleRange.first) * continuationCandidatesNoRadius[i]->averageLength));
				maxWidth = fmax(maxWidth, fabs((float)(sin(continuationCandidatesNoRadius[i]->averageAngle) * continuationCandidatesNoRadius[i]->averageLength)));
			}
		}
		else if (continuationCandidatesNoRadius.size() == 1)
		{
			continuation = continuationCandidatesNoRadius.front();
		}
		else if (continuationCandidatesRadius.size() == 1)
		{
			continuation = continuationCandidatesRadius.front();
			for (int i=0; i<continuationCandidatesNoRadius.size(); ++i)
			{
				if (continuationCandidatesNoRadius[i] != continuation)
				{
					copyChildren.push_back(make_pair(current->trunkHeight + current->averageLength * continuationCandidatesNoRadius[i]->relativePosition, continuationCandidatesNoRadius[i]));
					maxHeight = max(maxHeight, (float)(cos(continuationCandidatesNoRadius[i]->angleRange.first) * continuationCandidatesNoRadius[i]->averageLength));
					maxWidth = fmax(maxWidth, fabs((float)(sin(continuationCandidatesNoRadius[i]->averageAngle) * continuationCandidatesNoRadius[i]->averageLength)));
				}
			}
		}
		else if (continuationCandidatesNoRadius.size() > 0)
		{
			addLater = true;
			for (int i=0; i<continuationCandidatesNoRadius.size(); ++i)
			{
				maxHeight = max(maxHeight, (float)(cos(continuationCandidatesNoRadius[i]->angleRange.first) * continuationCandidatesNoRadius[i]->averageLength));
				maxWidth = fmax(maxWidth, fabs((float)(sin(continuationCandidatesNoRadius[i]->averageAngle) * continuationCandidatesNoRadius[i]->averageLength)));

				continuationCandidatesNoRadius[i]->complexRotationAroundTrunk();
			}
		}


		/*for (int i=0; i<current->childBranches.size(); ++i)
		{

			if (current->childBranches[i]->relativePosition >= BRANCH_CONTINUATION_THRESHOLD)
			{
				if (continuation == 0)
				{
					continuation = current->childBranches[i];
				}
				else
				{
					splits = true;
					//continuation->complexRotationAroundTrunk();
					continuation = current->childBranches[i];
				}
			}
			else
			{
				if (approxEqual(current->childBranches[i]->rotationalAngle,180.0f))
					westChildren.push_back(current->childBranches[i]);
				else
					eastChildren.push_back(current->childBranches[i]);
				maxHeight = max(maxHeight, (float)(cos(current->childBranches[i]->angleRange.first) * current->childBranches[i]->averageLength));
				maxWidth = fmax(maxWidth, fabs((float)(sin(current->childBranches[i]->averageAngle) * current->childBranches[i]->averageLength)));
			}

		}*/

	}   while (continuation != 0);


	maxHeight = trunkLength + maxHeight;

	//if (splits)
	//{
		//continuation->complexRotationAroundTrunk();
	//}

	sort(copyChildren.begin(), copyChildren.end());

	Branch* copy;
	vector<Branch*> toPlace;
	float groupStartPosition = 0.0f;
	int s = (int)copyChildren.size();
	float placement = 0.0f;
	for (int i=0; i<s; ++i)
	{
		copy = new Branch(*(copyChildren[i].second));
		//copyChildren[i]->parent->childBranches.push_back(copy);

		if (toPlace.empty())
		{
			toPlace.push_back(copy);
			groupStartPosition = copyChildren[i].first;
		}
		else if ((copyChildren[i].first - groupStartPosition) / trunkLength < BRANCH_CLUSTER_RANGE)
		{
			toPlace.push_back(copy);
		}
		else
		{
			placement = (groupStartPosition + copyChildren[i].first) / 2.0f;
			int n = (int)toPlace.size();
			for (int j=0; j<n; ++j)
			{
				int b = 0;
				float p = placement + copyChildren[i-n + j].first - groupStartPosition;
				while (b < trunk.size() && trunk[b]->trunkHeight < p)
				{
					++b;
				}
				--b;
				p -= trunk[b]->trunkHeight;
				toPlace[j]->relativePosition = fmin(p / trunk[b]->averageLength, BRANCH_CONTINUATION_THRESHOLD - 0.01);
				trunk[b]->childBranches.push_back(toPlace[j]);
			}

			toPlace.clear();
			toPlace.push_back(copy);
			groupStartPosition = copyChildren[i].first;
		}


		if (approxEqual(copyChildren[i].second->rotationalAngle,180.0f))
		{
			westChildren.push_back(copyChildren[i].second);
			southChildren.push_back(copy);

			copy->rotationalAngle = 270.0f;
		}
		else
		{
			eastChildren.push_back(copyChildren[i].second);
			northChildren.push_back(copy);

			copy->rotationalAngle = 90.0f;
		}
	}

	if (!toPlace.empty());
	{
		if (placement < 0.01f)
			placement = groupStartPosition / 2.0f;
		placement = groupStartPosition + fmin((trunkLength-groupStartPosition) / 2.0f, groupStartPosition-placement);
		int n = (int)toPlace.size();
		for (int j=0; j<n; ++j)
		{
			int b = 0;
			float p = placement + copyChildren[s-n + j].first - groupStartPosition;
			while (b < trunk.size() && trunk[b]->trunkHeight < p)
			{
				++b;
			}
			--b;
			p -= trunk[b]->trunkHeight;
			toPlace[j]->relativePosition = fmin(p / trunk[b]->averageLength, BRANCH_CONTINUATION_THRESHOLD - 0.01);
			trunk[b]->childBranches.push_back(toPlace[j]);
		}

	}



	random_shuffle(southChildren.begin(), southChildren.end());
	random_shuffle(northChildren.begin(), northChildren.end());
	random_shuffle(eastChildren.begin(), eastChildren.end());
	random_shuffle(westChildren.begin(), westChildren.end());


	float*** distanceField = new float**[DISTANCE_FIELD_SIZE+1];

	for (int i=0; i<=DISTANCE_FIELD_SIZE; ++i)
	{
		distanceField[i] = new float*[DISTANCE_FIELD_SIZE+1];
		for (int j=0; j<=DISTANCE_FIELD_SIZE; ++j)
		{
			distanceField[i][j] = new float[DISTANCE_FIELD_VERTICAL_SIZE+1];
			for (int k=0; k<=DISTANCE_FIELD_VERTICAL_SIZE; ++k)
			{
				distanceField[i][j][k] = INF;
			}
		}
	}



	if (addLater)
	{
		random_shuffle(continuationCandidatesNoRadius.begin(), continuationCandidatesNoRadius.end());
		for (int i=0; i<continuationCandidatesNoRadius.size(); ++i)
		{
			insertAtBestPosition(continuationCandidatesNoRadius[i], distanceField, maxHeight, maxWidth, true);
		}
	}

	int m = 0;
	m = max(m, (int)(southChildren.size()));
	m = max(m, (int)(northChildren.size()));
	m = max(m, (int)(eastChildren.size()));
	m = max(m, (int)(westChildren.size()));


	for (int i=0; i<m; ++i)
	{
		if (i < southChildren.size())
		{
					insertAtBestPosition(southChildren[i], distanceField, maxHeight, maxWidth, false);
		}
		if (i < northChildren.size())
		{
					insertAtBestPosition(northChildren[i], distanceField, maxHeight, maxWidth, false);
		}
		if (i < eastChildren.size())
		{
					insertAtBestPosition(eastChildren[i], distanceField, maxHeight, maxWidth, false);
		}
		if (i < westChildren.size())
		{
					insertAtBestPosition(westChildren[i], distanceField, maxHeight, maxWidth, false);
		}
	}


	for (int i=0; i<=DISTANCE_FIELD_SIZE; ++i)
	{
		for (int j=0; j<=DISTANCE_FIELD_SIZE; ++j)
		{
			delete[] distanceField[i][j];
		}
		delete[] distanceField[i];
	}
	delete[] distanceField;

}



