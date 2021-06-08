
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

Key HTMSubTree::getSTARELEVELCode(STARE_ArrayIndexSpatialValue key, int level){
    if (level == 0){
        return (key & 0x3800000000000000);
    }
    else if(level > 0 && level < 28){
        Key mask = 0x3fffffffffffffe0;
        mask = mask << ((27 - level)*2);
        //mask = mask | 0x000000000000001f;
        mask = mask & 0x3fffffffffffffff;
        // mask = {00} {111} {[11|00]^27} {00000} 
        return (key & mask);
        //Key mask = 0x0600000000000000;
        //mask = mask >> (level * 2);
        //return (key & mask);
    }
    else{        
        std::cout << "Input Error (getSTARELEVELCode): Stare level is out of range (i.e., [0, 27])!"
        return -1;
    }
}

// Return a list of STARE_ENCODE
std::list<STARE_ENCODE>* HTMSubTree::intersect(HTMSubTreeNode* Ins_root){
    std::list<STARE_ENCODE>* result = NULL;
    HTMSubTreeNode* sub_Ins_root = getHighestRoot(Ins_root);            // sub_Ins_root: the highest root of a given tree.
    HTMSubTreeNode* sub_root = getPotentialBranch(root, sub_Ins_root);  //sub_root: the potential branch from root
                                                                        //sub_Ins_root and sub_root have the same level. 
    if (sub_root == NULL || sub_Ins_root == NULL){
        std::cout << "No intersection!";
        return NULL; //There is no intersection
    }

    rec_intersect(sub_root, sub_Ins_root, result);
    return result;
} 

//root_a and root_b should have the same key and same level
int HTMSubTree::rec_intersect(HTMSubTreeNode* root_a, HTMSubTreeNode* root_b, std::list<STARE_ENCODE> * result){
    if(root_a->key != root_b->key || root_a->level != root_b->level) {
        std::cout << "Input Error (rec_intersect): two sub_roots are not match each other!";
        return -1;
    }
    if((root_a->isLeaf)  || (root_b->isLeaf)){
        Key res_key = root_a->key;
        if (root_b->isLeaf)
            res_key = root_b->key;
        result->push_back(res_key); //add key of a leaf
        return 1;
    }
    else{//Both root_a and root_b are not leaf-node
        int loop = MAX_NUM_CHILD_II;
        if (sub_root->level == 0)
            loop = MAX_NUM_CHILD;
        int res = 0;
        for (int i = 0; i < loop; i++){
            res = rec_intersect(root_a->entries[i], root_b->entries[i], result);
            if(res < 0) 
                break; //Stop if find any error
        }
        return res;
    }
}

HTMSubTreeNode* HTMSubTree::getHighestRoot(HTMSubTreeNode* Ins_root){
    if(Ins_root == NULL){
        std::cout << "Input Error (getHighestRoot): The input is NULL!";
        return NULL;
    }
    if(Ins_root->isLeaf || Ins_root->count > 1){
        return Ins_root;
    }
    int loop = MAX_NUM_CHILD_II;
    if (sub_root->level == 0)
        loop = MAX_NUM_CHILD;
    for(int i = 0; i < loop; i++){ // There is only one non-NULL child
        if(Ins_root->entries[i].child != NULL)
            return getHighestRoot(Ins_root->entries[i].child);
    }
    std::cout << "Input Error (getHighestRoot): All children are NULL!";
    return Ins_root;
}

HTMSubTreeNode* HTMSubTree::getPotentialBranch(HTMSubTreeNode* root_a, HTMSubTreeNode* root_b){
    if(root_a == NULL || root_b == NULL){
        std::cout << "Input Error (getPotentialBranch): The input is NULL!";
        return NULL;
    }

    if(root_a->level > root_b->level){
        std::cout << "Input Error (getPotentialBranch): A tree is smaller than B Tree. Please check the order of parameters!";
        return NULL;
    }
    else if(root_a->level == root_b->level){
        if(root_a->key == root_b->key)
            return root_a;
        else {
            std::cout << "Keys are not the same!";
            return NULL;
        }
    } 
    else{        
        int loop = MAX_NUM_CHILD_II;
        if (sub_root->level == 0)
            loop = MAX_NUM_CHILD;
        for (int i = 0; i < loop; i++){
            if(root_a->entries[i].key == root_b->key){
                if(root_a->entries[i].child != NULL)
                    return getPotentialBranch(root_a->entries[i].child, root_b);
                else
                    return NULL;
            }
        }
        std::cout << "Input Error (getPotentialBranch): List of entry keys are not correct!";
        return NULL;
    }
}