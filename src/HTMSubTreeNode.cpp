#include "HTMSubTreeNode.h"

HTMSubTreeNodeEntry::HTMSubTreeNodeEntry(){
    child = NULL;
    key = 0;
}

HTMSubTreeNodeEntry::~HTMSubTreeNodeEntry(){
    if(child != NULL){
        delete child;
        child = NULL;
    }
}

HTMSubTreeNode::HTMSubTreeNode(){
    key = 0;
    isLeaf = True;
    level = 0;
    count = 0;
    for (i = 0; i < MAX_NUM_CHILD; i++){
        entries[i] = new HTMSubTreeNodeEntry();
    }
}

HTMSubTreeNode::~HTMSubTreeNode(){    
    key = 0;
    isLeaf = True;
    level = 0;
    count = 0;
    for (i = 0; i < MAX_NUM_CHILD; i++){
        delete entries[i];
    }
}