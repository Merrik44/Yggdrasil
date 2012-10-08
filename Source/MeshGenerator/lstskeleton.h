#ifndef LSTSKELETON_H
#define LSTSKELETON_H
#include "string"
#include "vector"
#include "vmath.h"
#include "branchnode.h"

class BranchNode;
class LstSkeleton
{
public:
    LstSkeleton();
    LstSkeleton(std::string filepath);
    ~LstSkeleton( );
    void LoadLstFile(std::string filepath );
    void Draw();
    bool TryCollapseBranch( BranchNode* branch );
    void CalculateBranchOffsets( BranchNode& branch);
    void SimplifyGraph();
    void createRandomBifurcation(int numberOfBranches, float branchLength, float maxRadius, float minRadius );


    float scale;
    std::string filepath;

    BranchNode* root;
    std::vector< BranchNode* > branches;
    std::vector< BranchNode* > branchTips;

};

#endif // LSTSKELETON_H
