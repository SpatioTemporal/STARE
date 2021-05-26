
#include "HTMSubTree.h"

HTMSubTree::HTMSubTree(){
    root = new HTMSubTreeNode();
    //for (int64 i = 0; i < MAX_NUM_CHILD; i++){
    //    root[i] = new HTMSubTreeNodeEntry(i);
    //}
}

HTMSubTree::~HTMSubTree(){
    if(root != NULL){
        delete root;
    }
}

HTMSubTree::HTMSubTree(string sidecar){
    //TODO: load HTM SubTree from sidecar file
    // Read the list of STARE values from file 
    // and rcall HTMSubTree(STARE_SpatialInterval sids)
}

HTMSubTree::HTMSubTree(STARE_SpatialIntervals sids){
    if(!sids.empty()){
        size = sids.size();
        //HTMSubTreeNode* curNode = NULL;
        for (int i = 0; i < size; i++){
            //curNode = root;
            curSID = sids[i];
            addSTAREID(curSID);
        }
    }
    else{
        std::cout << "Input Error: The list of STARE values is empty!"
    }
}

void HTMSubTree::addSTAREID(STARE_ArrayIndexSpatialValue key){
    HTMSubTreeNode* curNode = root;
    int level = key & 0x000000000000000f;
    int curCode = 0;
    for (int i = 0; i <= level; i++){
        curCode = getSTARELEVELCode(key, i);
        curNode = createChildNode(curNode, curCode, i);
    }
}

HTMSubTreeNode* HTMSubTree::createChildNode(HTMSubTreeNode* current, int64 code, int level){
    if(level >= 0 and level < 28){
        pos = code >> (5 + (27 - level) * 2);
        if(current.entries[pos] == NULL){    
            newNode = HTMSubTreeNode(code, true, level, 0);
            current.entries[pos].child = newNode;
            current.entries[pos].key = code;
            current.count += 1;
            current.isLeaf = false;
        }
        return current.entries[pos].child;
    }
    else{
        std::cout << "Input Error (createChildNode): Stare level is out of range (i.e., [0, 27])!"
        return NULL;
    }
}

int HTMSubTree::getSTARELEVELCode(STARE_ArrayIndexSpatialValue key, int level){
    if (level == 0){
        return (key & 0x3800000000000000);
    }
    else if(level > 0 && level < 28){
        int mask = 0x0600000000000000;
        mask = mask >> (level * 2);
        return (key & mask);
    }
    else{        
        std::cout << "Input Error (getSTARELEVELCode): Stare level is out of range (i.e., [0, 27])!"
        return -1;
    }
}