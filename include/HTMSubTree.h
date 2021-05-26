
#ifndef INCLUDE_HTMSUBTREE_H_
#define INCLUDE_HTMSUBTREE_H_

#include "HTMSubTreeNode.h"

class HTMSubTree{
public:
    // Contructors
    HTMSubTree();    
    HTMSubTree(string sidecar);//Load HTMSubTree from sidecar file
    HTMSubTree(STARE_SpatialIntervals sids);//Load HTMSubTree from list of STARE values

    // Destructors
    ~HTMSubTree();

    // Attributes
    HTMSubTreeNode* root;
    //HTMSubTreeNodeEntry* root[MAX_NUM_CHILD];

    // Methods
    bool isIntersect(HTMSubTreeNode* Ins_root);
    bool isContain(STARE_ArrayIndexSpatialValue siv);

    STARE_ENCODE* intersect(HTMSubTreeNode* Ins_root); // Return a list of STARE_ENCODE

    void printTree();
    
private:
    HTMSubTreeNode*    createChildNode(HTMSubTreeNode* current, int64 code, int level);
    void    addSTAREID(STARE_ArrayIndexSpatialValue key);
    // Helper functions
    int     getSTARELEVELCode(STARE_ArrayIndexSpatialValue key, int level);
}
#endif /* INCLUDE_HTMSUBTREE_H_ */