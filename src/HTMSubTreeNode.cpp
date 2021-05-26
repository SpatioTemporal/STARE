#include "HTMSubTreeNode.h"

HTMSubTreeNodeEntry::HTMSubTreeNodeEntry(){
    child = NULL;
    key = 0;
}

HTMSubTreeNodeEntry::HTMSubTreeNodeEntry(Key _key){
    child = NULL;
    key = _key;
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
        entries[i] = new HTMSubTreeNodeEntry(i);
    }
}

HTMSubTreeNode::HTMSubTreeNode(Key _key, bool _isLeaf, int _level, int _count){
    key = _key;
    isLeaf = _isLeaf;
    level = _level;
    count = _count;
    int loop = MAX_NUM_CHILD;
    if(_level > 0)
        loop = MAX_NUM_CHILD_II;
    for (i = 0; i < loop; i++){
        entries[i] = new HTMSubTreeNodeEntry();
    }
}

HTMSubTreeNode::~HTMSubTreeNode(){    
    key = 0;
    isLeaf = true;
    level = 0;
    count = 0;
    for (i = 0; i < MAX_NUM_CHILD; i++){
        delete entries[i];
    }
}