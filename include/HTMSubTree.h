
#ifndef INCLUDE_HTMSUBTREE_H
#define INCLUDE_HTMSUBTREE_H

#include "HTMSubTreeNode.h"
#include <list>
#include <iostream>
#include <string>
#include <vector>

typedef unsigned long long STARE_ArrayIndexSpatialValue;
//typedef std::vector<STARE_ArrayIndexSpatialValue> STARE_SpatialIntervals;
typedef std::vector<STARE_ArrayIndexSpatialValue> STARE_SpatialIntervals;

class HTMSubTree{
public:
    // Contructors
    HTMSubTree();    
    HTMSubTree(char* sidecar);//Load HTMSubTree from sidecar file
    HTMSubTree(STARE_SpatialIntervals sids);//Load HTMSubTree from list of STARE values

    // Destructors
    ~HTMSubTree();

    // Attributes
    HTMSubTreeNode* root;
    //HTMSubTreeNodeEntry* root[MAX_NUM_CHILD];

    // Methods
    bool isIntersect(HTMSubTreeNode* Ins_root);
    bool isContain(STARE_ArrayIndexSpatialValue siv);

    std::list<STARE_ENCODE>* intersect(HTMSubTreeNode* Ins_root); // Return a list of STARE_ENCODE

    void printTree();

    void printFromNode(HTMSubTreeNode* current);
    
//private:
    HTMSubTreeNode*     createChildNode(HTMSubTreeNode* current, STARE_ArrayIndexSpatialValue code, int level);
    void                addSTAREID(Key key);
    // Helper functions
    Key                 getSTARELEVELCode(Key key, int level);
    int                 rec_intersect(HTMSubTreeNode* root_a, HTMSubTreeNode* root_b, std::list<STARE_ENCODE> * result);
    HTMSubTreeNode*     getHighestRoot(HTMSubTreeNode* Ins_root);
    HTMSubTreeNode*     getPotentialBranch(HTMSubTreeNode* root_a, HTMSubTreeNode* root_b);
};


#endif /* INCLUDE_HTMSUBTREE_H_ */