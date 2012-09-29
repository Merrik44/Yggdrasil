/*
 * branch.h
 *
 *  Created on: Aug 23, 2011
 *      Author: Mark
 */

#ifndef BRANCH_H_
#define BRANCH_H_

#include <algorithm>
#include <vector>
#include <rapidxml.hpp>

class Branch
{
    public:
        Branch();
        virtual ~Branch();
        Branch(const Branch& other);

        std::pair<float,float> angleRange;
        std::pair<float,float> startThicknessRange;
        std::pair<float,float> endThicknessRange;
        std::pair<float,float> lengthRange;
        float relativePosition;
        float rotationalAngle;

        std::vector<Branch*> childBranches;

        Branch* parent;

        static Branch* constructTreeFromXML(const rapidxml::xml_document<> *document);

        void writeTreeToXMLDocument(rapidxml::xml_document<> *document);

        static void convertTo3D(Branch* root);


        float flattenedLength;
        float averageLength;
        float averageAngle;
        float averageStartThickness;
        float averageEndThickness;
        float trunkHeight;

    protected:
    private:
        bool isSubSegment;

        void flipBranch();

        void writeBranchToXMLDocument(rapidxml::xml_document<> *document, rapidxml::xml_node<> *branch);
        static Branch* constructBranchFromXML(const rapidxml::xml_node<> *node, Branch* parent);
        void preprocessBranches();
        void simpleRotationAroundBranches(bool isTrunk);
        void complexRotationAroundTrunk();

        void clean();

};


#endif /* BRANCH_H_ */
