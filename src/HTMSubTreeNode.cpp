#include "HTMSubTreeNode.h"

/*HTMSubTreeNodeEntry::HTMSubTreeNodeEntry(){
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
}*/

HTMSubTreeNode::HTMSubTreeNode(){
    key = 0;
    isLeaf = true;
    level = 0;
    count = 0;
    for (int i = 0; i < MAX_NUM_CHILD; i++){
        children[i] = NULL;
        keys[i] = genSTARELEVELCode(0, 0, i);
    }
}

HTMSubTreeNode::HTMSubTreeNode(STARE_ENCODE _key, bool _isLeaf, int _level, int _count){
    key = _key;
    isLeaf = _isLeaf;
    level = _level;
    count = _count;
    int loop = MAX_NUM_CHILD;
    if(_level > 0)
        loop = MAX_NUM_CHILD_II;
    for (int i = 0; i < loop; i++){
        children[i] = NULL;
        keys[i] = genSTARELEVELCode(_key, _level, i);
        //std::cout << "===" << std::hex << keys[i] << "==== \n";
    }
}

STARE_ENCODE HTMSubTreeNode::genSTARELEVELCode(STARE_ENCODE key, unsigned int level, unsigned int pos){
    STARE_ENCODE result = 0;
    pos = pos % 8;
    STARE_ENCODE temp = ((unsigned long long)pos);
    if (level == 0){        
        result = temp << (5 + 27 * 2);
        return result;
    }
    else if(level > 0 && level < 28){
        STARE_ENCODE mask = 0x3fffffffffffffe0;
        mask = mask << ((27 - (level - 1))*2);
        mask = mask & 0x3fffffffffffffff; 
        result = key & mask; 
        result = result | ((temp) << (5 + (27 - level) * 2));
        return result;
    }
    else{        
        std::cout << "Input Error (getSTARELEVELCode): Stare level is out of range (i.e., [0, 27])!";
        return result;
    }
}

HTMSubTreeNode::~HTMSubTreeNode(){    
    key = 0;
    isLeaf = true;
    count = 0;
    int loop = MAX_NUM_CHILD;
    if(level > 0)
        loop = MAX_NUM_CHILD_II;
    level = 0;
    for (int i = 0; i < loop; i++){
        if(children[i] != NULL){
            delete children[i];
            children[i] = NULL;
        }
        keys[i] = 0;
    }
}