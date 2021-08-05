
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

HTMSubTree::HTMSubTree(char* sidecar){
    //TODO: load HTM SubTree from sidecar file
    // Read the list of STARE values from file 
    // and rcall HTMSubTree(STARE_SpatialInterval sids)
}

HTMSubTree::HTMSubTree(STARE_SpatialIntervals sids){
    if(!sids.empty()){
        int size = sids.size();
        unsigned long long curSID = 0;
        root = new HTMSubTreeNode();
        root->isLeaf = false;//start to insert
        for (int i = 0; i < size; i++){
            curSID = sids[i];
            addSTAREID(curSID);
        }
    }
    else{
        std::cout << "Input Error: The list of STARE values is empty!";
    }
}

void HTMSubTree::addSTAREID(STARE_ENCODE key){
    HTMSubTreeNode* curNode = root;
    unsigned long long level = key & 0x000000000000001f;
    STARE_ENCODE curCode = 0;
    std::list<HTMSubTreeNode*> *path = new std::list<HTMSubTreeNode*>();
    //path->push_back(curNode); // we don't group leaves in the level 0
    for (int i = 0; i <= level; i++){
        curCode = getSTARELEVELCode(key, i);
        int loop = MAX_NUM_CHILD_II;
        if (i == 0)
            loop = MAX_NUM_CHILD;
        for(int j = 0; j < loop; j++){
            if(curNode->isLeaf){ //There are already a bigger leaf node in the subtree
                path->clear();
                delete path;
                return;//Should return instead of going to the next level
            }
            if(curCode == curNode->keys[j]){
                if(curNode->children[j] == NULL){
                    HTMSubTreeNode* temp = new HTMSubTreeNode(curCode, true, i + 1, 0);
                    curNode->children[j] = temp;
                    curNode->count += 1;
                    if(i < level){
                        temp->isLeaf = false;//Continue to go further
                        curNode = temp;
                        path->push_back(curNode);
                    }
                    else if(i == level){ //Set the level for the key
                        temp->key = temp->key | ((unsigned long long)level);
                        //temp->key = key; // ---- This can be faster ----
                        if(!path->empty())
                            path->pop_back();
                        tryGroupLeaves(curNode, path);
                        path->clear();
                        delete path;
                        return;
                    }
                }else{
                    if(i < level){
                        curNode->isLeaf = false;
                        curNode = curNode->children[j];//go further
                        path->push_back(curNode);
                    }
                    else if(i == level){//final level of the key, going to insert the key as a leaf
                        //Remove all children in level i+1 of the subtree
                        int loop1 = MAX_NUM_CHILD_II;
                        if ((i + 1) == 0) // never happen
                            loop1 = MAX_NUM_CHILD;
                        for (int t = 0; t < loop1; t++){
                            if(curNode->children[j]->children[t] != NULL){ 
                                delete curNode->children[j]->children[t];
                                curNode->children[j]->children[t] = NULL;
                            }   
                        }
                        curNode->children[j]->isLeaf = true;
                        curNode->children[j]->count = 0;
                        curNode->children[j]->key = curNode->children[j]->key | ((unsigned long long)level);
                        //curNode->children[j]->key = key; // ---- This can be faster ----
                        if(!path->empty())
                            path->pop_back();
                        tryGroupLeaves(curNode, path);
                        path->clear();
                        delete path;
                        return;
                    }
                }
                break;//go to the next level of the key
            }
        }
    }
    path->clear();
    delete path;
    return;
}

void HTMSubTree::tryGroupLeaves(HTMSubTreeNode* curNode, std::list<HTMSubTreeNode*> *path){
    if (curNode == NULL){
        std::cout << "Input Error (tryGroupLeaves): The curNode is NULL!";
        return;
    }
    int l = (curNode->level == 0) ? MAX_NUM_CHILD : MAX_NUM_CHILD_II;
    if(curNode->count == l){
        //Check all children to see if any of them is not a leaf
        for(int i = 0; i < l; i++){
            if (!curNode->children[i]->isLeaf)
                return; //if there is a non-leaf child, then do nothing.
        }
        //ALl children are leaves --- remove all children
        for (int t = 0; t < l; t++){
            if(curNode->children[t]!= NULL){ 
                delete curNode->children[t];
                curNode->children[t] = NULL;
            }   
        }
        curNode->isLeaf = true;
        curNode->count = 0;
        if(curNode->level > 0)
            curNode->key = curNode->key | ((unsigned long long)(curNode->level - 1));
        if(!path->empty()){
            HTMSubTreeNode* temp = path->back();
            path->pop_back();
            tryGroupLeaves(temp, path);
        }
    }
}

HTMSubTreeNode* HTMSubTree::createChildNode(HTMSubTreeNode* current, STARE_ArrayIndexSpatialValue code, int level){
    if(level >= 0 and level < 28){
        int pos = code >> (5 + (27 - level) * 2);
        if(current->children[pos] == NULL){    
            HTMSubTreeNode* newNode = new HTMSubTreeNode(code, true, level, 0);
            current->children[pos] = newNode;
            current->keys[pos] = code;
            current->count += 1;
            current->isLeaf = false;
        }
        return current->children[pos];
    }
    else{
        std::cout << "Input Error (createChildNode): Stare level is out of range (i.e., [0, 27])!";
        return NULL;
    }
}

STARE_ENCODE HTMSubTree::getSTARELEVELCode(STARE_ENCODE key, int level){
    if (level == 0){
        return (key & 0x3800000000000000);
    }
    else if(level > 0 && level < 28){
        STARE_ENCODE mask = 0x3fffffffffffffe0;
        mask = mask << ((27 - level)*2);
        mask = mask & 0x3fffffffffffffff; 
        return (key & mask);
    }
    else{        
        std::cout << "Input Error (getSTARELEVELCode): Stare level is out of range (i.e., [0, 27])!";
        return 0;
    }
}

// Return a list of STARE_ENCODE
std::list<STARE_ENCODE>* HTMSubTree::intersect(HTMSubTreeNode* Ins_root){
    std::list<STARE_ENCODE> *result = new std::list<STARE_ENCODE>();
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
    STARE_ENCODE key_a = root_a->key & 0x3fffffffffffffe0; //clear level;
    STARE_ENCODE key_b = root_b->key & 0x3fffffffffffffe0;
    if(key_a != key_b || root_a->level != root_b->level) {
        std::cout << "Input Error (rec_intersect): two sub_roots are not match each other!";
        return -1;
    }
    if((root_a->isLeaf)  || (root_b->isLeaf)){
        STARE_ENCODE res_key = root_a->key;
        if (root_b->isLeaf)
            res_key = root_b->key;
        result->push_back(res_key); //add key of a leaf
        return 1;
    }
    else{//Both root_a and root_b are Non-Leaf nodes
        int loop = MAX_NUM_CHILD_II;
        if (root_a->level == 0)
            loop = MAX_NUM_CHILD;
        int res = 0;
        for (int i = 0; i < loop; i++){
            if((root_a->children[i] != NULL) && (root_b->children[i] != NULL)){
                res = rec_intersect(root_a->children[i], root_b->children[i], result);
                if(res < 0) 
                    break; //Stop if find any error
            }                
        }
        return res;
    }
}

bool HTMSubTree::isIntersect(HTMSubTreeNode* Ins_root){ // return if there is any overlapping
    bool result = false;
    HTMSubTreeNode* sub_Ins_root = getHighestRoot(Ins_root);            // sub_Ins_root: the highest root of a given tree.
    HTMSubTreeNode* sub_root = getPotentialBranch(root, sub_Ins_root);  //sub_root: the potential branch from root
                                                                        //sub_Ins_root and sub_root have the same level. 
    if (sub_root == NULL || sub_Ins_root == NULL)
        return result; //There is no intersection
    
    result = rec_isIntersect(sub_root, sub_Ins_root);
    return result;
}

bool HTMSubTree::rec_isIntersect(HTMSubTreeNode* root_a, HTMSubTreeNode* root_b){
    bool result = false;
    STARE_ENCODE key_a = root_a->key & 0x3fffffffffffffe0; //clear level;
    STARE_ENCODE key_b = root_b->key & 0x3fffffffffffffe0;
    if(key_a != key_b || root_a->level != root_b->level) {
        std::cout << "Input Error (rec_isIntersect): two sub_roots are not match each other!";
        return false;
    }
    if((root_a->isLeaf)  || (root_b->isLeaf)){
        STARE_ENCODE res_key = root_a->key;
        if (root_b->isLeaf)
            res_key = root_b->key;
        //result->push_back(res_key); //add key of a leaf
        return true; // found an overlapping leaf
    }
    else{//Both root_a and root_b are Non-Leaf nodes
        int loop = MAX_NUM_CHILD_II;
        if (root_a->level == 0)
            loop = MAX_NUM_CHILD;
        for (int i = 0; i < loop; i++){
            if((root_a->children[i] != NULL) && (root_b->children[i] != NULL)){
                result = rec_intersect(root_a->children[i], root_b->children[i], result);
                if(result) 
                    return result; //Stop if find any overlapping leaf
            }                
        }
        return result;
    }
}

bool HTMSubTree::isContain(STARE_ArrayIndexSpatialValue siv){ //return if siv is contained in HTMSubTree
    bool result = false;
    HTMSubTreeNode* sub_root = getHighestRoot(root);            // sub_root: the highest root of a given tree.
    if (sub_root == NULL)
        return result; //siv is not contained in the tree
    
    result = rec_isContain(sub_root, siv);
    
    return result;
}

bool HTMSubTree::rec_isContain(HTMSubTreeNode* sub_root, STARE_ArrayIndexSpatialValue siv){
    if(sub_root == NULL){
        std::cout << "Input Error (rec_isContain): The input is NULL!";
        return false;
    }
    if(sub_root->isLeaf){
        return check_Contain(sub_root->key, siv);
    }
    else{//Both sub_root is Non-Leaf nodes
        unsigned long long level_r = sub_root->key & 0x000000000000001f;
        unsigned long long level_siv = siv & 0x000000000000001f;
        if(level_siv <= level_r)//siv's coverage is larger (or equal) than sub_root's coverage
            return false;
        else{ // level_siv > level_r: siv's coverage is smaller than sub_root's coverage
            STARE_ENCODE code_sid_adjust = getSTARELEVELCode(siv, level_r + 1);
            int loop = MAX_NUM_CHILD_II;
            if (sub_root->level == 0)
                loop = MAX_NUM_CHILD;
            
            for (int i = 0; i < loop; i++){
                if((sub_root->keys[i] == code_sid_adjust) && (root_b->children[i] != NULL)){
                    result = rec_isContain(sub_root->children[i], siv);
                    return result; //return the result from the only one potential child that may cover siv
                }                
            }
            return false; //Cannot find any potential child.    
        }
    }
}

//Check if STARE_ID_b is contained in STARE_ID_a
bool HTMSubTree::check_Contain(STARE_ENCODE key_a, STARE_ENCODE key_b){
    unsigned long long level_a = key_a & 0x000000000000001f;
    unsigned long long level_b = key_b & 0x000000000000001f;
    if(level_b < level_a)//b's coverage is larger than a's coverage
        return false;
    else {// level_b >= level_a: b's coverage is smaller than a's coverage
        STARE_ENCODE sid_a = getSTARELEVELCode(key_a, level_a);
        STARE_ENCODE sid_b_adjust = getSTARELEVELCode(key_b, level_a);
        if(sid_b_adjust == sid_a) // b in the region covered by a.
            return true;
        return false;
    }
}
std::list<list<STARE_ENCODE>>* HTMSubTree::leftJoin(HTMSubTree* Ins_root){
    return NULL;
}
std::list<list<STARE_ENCODE>>* HTMSubTree::innerJoin(HTMSubTree* Ins_root){
    return NULL;
}
std::list<list<STARE_ENCODE>>* HTMSubTree::fullJoin(HTMSubTree* Ins_root){
    return NULL;
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
    if (Ins_root->level == 0)
        loop = MAX_NUM_CHILD;
    for(int i = 0; i < loop; i++){ // There is only one non-NULL child
        if(Ins_root->children[i] != NULL)
            return getHighestRoot(Ins_root->children[i]);
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
        STARE_ENCODE key_a = root_a->key & 0x3fffffffffffffe0; //clear level;
        STARE_ENCODE key_b = root_b->key & 0x3fffffffffffffe0;
        if(key_a == key_b)
            return root_a;
        else {
            std::cout << "Keys are not the same!";
            return NULL;
        }
    } 
    else{        
        int loop = MAX_NUM_CHILD_II;
        if (root_a->level == 0)
            loop = MAX_NUM_CHILD;
        STARE_ENCODE curCode = getSTARELEVELCode(root_b->key, root_a->level);
        for (int i = 0; i < loop; i++){
            if(root_a->keys[i] == curCode){
                if(root_a->children[i] != NULL)
                    return getPotentialBranch(root_a->children[i], root_b);
                else
                    return NULL;
            }
        }
        std::cout << "Input Error (getPotentialBranch): List of entry keys are not correct!";
        std::cout << "\nLevel: " << root_a->level;
        return NULL;
    }
}
//Print the keyCode (i.e. keys)
void HTMSubTree::printTree(){
    if(root == NULL){
        std::cout << "Error: The root is NULL!";
    }
    else{
        printFromNode(root);
    }
}

void HTMSubTree::printFromNode(HTMSubTreeNode* current){
    if(current != NULL){
        int loop = MAX_NUM_CHILD_II;
        if(current->level == 0)
            loop = MAX_NUM_CHILD;
        if(current->level == 0){
            std::cout << std::hex << "Root:  " << current->key << "\tcount: " << current->count;
            if(current->isLeaf) 
                std::cout << "\tLeaf \n";
            else
                std::cout << "\tNon-Leaf \n";
        }
        for (int i = 0; i < loop; i++){
            if(current->children[i] != NULL){
                std::cout << (current->level) << ":";
                for(int j = 0; j < (current->level + 1); j++)
                    std::cout << "  ";
                //std::cout << current->keys[i] << "\tcount: "<< current->children[i]->count;
                if(current->children[i]->isLeaf){
                    std::cout << current->children[i]->key << "\tcount: "<< current->children[i]->count;
                    std::cout << "\tLeaf \n";
                }
                else {
                    std::cout << current->keys[i] << "\tcount: "<< current->children[i]->count;
                    std::cout << "\tNon-Leaf \n";
                }
                printFromNode(current->children[i]);
            }
        }
    }
}

void HTMSubTree::printTree2File(char* filename){
    if(root == NULL){
        std::cout << "Error: The root is NULL!";
    }
    else{
        if(filename == NULL){
            std::cout << "Error: The output filename is NULL!";
        }
        else{
            ofstream outputstream;
            outputstream.open(filename);
            printFromNode2File(root, outputstream);
            outputstream.close();
        }
    }
}
void HTMSubTree::printFromNode2File(HTMSubTreeNode* current, ofstream& outstream){
    if(current != NULL){
        int loop = MAX_NUM_CHILD_II;
        if(current->level == 0)
            loop = MAX_NUM_CHILD;
        if(current->level == 0){
            outstream << std::hex << "Root:  " << current->key << "\tcount: " << current->count;
            if(current->isLeaf) 
                outstream << "\tLeaf \n";
            else
                outstream << "\tNon-Leaf \n";
        }
        for (int i = 0; i < loop; i++){
            if(current->children[i] != NULL){
                outstream << (current->level) << ":";
                for(int j = 0; j < (current->level + 1); j++)
                    outstream << "  ";
                //outstream << current->keys[i] << "\tcount: "<< current->children[i]->count;
                if(current->children[i]->isLeaf){
                    outstream << current->children[i]->key << "\tcount: "<< current->children[i]->count;
                    outstream << "\tLeaf \n";
                }
                else{
                    outstream << current->keys[i] << "\tcount: "<< current->children[i]->count;
                    outstream << "\tNon-Leaf \n";
                }
                printFromNode2File(current->children[i], outstream);
            }
        }
    }
}

/* ========== <Timer class> =========== */
timer::timer()
{
    result = 0.0;
    resultInSecond = 0.0;
    resultInMilliSecond = 0.0;
    resultInMicroSecond = 0.0;
}

//This function starts the timer. 
void timer::start()
{
    startCounter = std::chrono::system_clock::now();
}

//This function stops the timer from running and records the time passed.
void timer::end()
{
    endCounter = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = endCounter - startCounter;
    result += elapsed.count();
    resultInSecond = result;
    resultInMilliSecond = result * 1000.0;
    resultInMicroSecond = result * 1000000.0;
}

//This function sets the timer to zero and restarts it again.
void timer::reStart()
{
    startCounter = std::chrono::system_clock::now();
    endCounter = startCounter;
    result = 0.0;
    resultInSecond = 0.0;
    resultInMilliSecond = 0.0;
    resultInMicroSecond = 0.0;
}

//This function is used to restarts the timer at a specific time.
void timer::reStartValue(double value)
{
    startCounter = std::chrono::system_clock::now();
    endCounter = startCounter;
    result = value;
    resultInSecond = value;
    resultInMilliSecond = value * 1000.0;
    resultInMicroSecond = value * 1000000.0;
}


double timer::getResult()
{
    return result;
}

double timer::getResultInSecond()
{
    return resultInSecond;
}

double timer::getResultInMilliSecond()
{
    return resultInMilliSecond;
}

double timer::getResultInMicroSecond()
{
    return resultInMicroSecond;
}
