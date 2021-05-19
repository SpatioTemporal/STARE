
#ifndef INCLUDE_HTMSUBTREE_H_
#define INCLUDE_HTMSUBTREE_H_

#include "HTMSubTreeNode.h"

class HTMSubTree{
public:
    // Contructors
    HTMSubTree();    
    HTMSubTree(string sidecar);//Load HTMSubTree from sidecar file

    // Destructors
    ~HTMSubTree();

    // Attributes
    HTMSubTreeNode* root;

    // Methods
    bool isIntersect(HTMSubTreeNode* Ins_root);

    STARE_ENCODE* intersect(HTMSubTreeNode* Ins_root); // Return a list of STARE_ENCODE
    
private:
}
#endif /* INCLUDE_HTMSUBTREE_H_ */