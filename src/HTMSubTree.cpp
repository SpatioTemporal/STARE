
#include "HTMSubTree.h"

HTMSubTree::HTMSubTree(){
    root = new HTMSubTreeNode();
}

HTMSubTree::~HTMSubTree(){
    if(root != NULL){
        delete root;
    }
}

HTMSubTree::HTMSubTree(string sidecar){
    //TODO: load HTM SubTree from sidecar file
}