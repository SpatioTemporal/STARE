
#ifndef INCLUDE_HTMSUBTREE_H
#define INCLUDE_HTMSUBTREE_H

#include "HTMSubTreeNode.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "STARE.h"

#include <cassert>
#include <chrono>
#include <cstdlib>

//These types are already defined in STARE.h
//typedef unsigned long long STARE_ArrayIndexSpatialValue;
//typedef std::vector<STARE_ArrayIndexSpatialValue> STARE_SpatialIntervals;

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

    void printTree2File(char* filename);
    void printFromNode2File(HTMSubTreeNode* current, ofstream& outputstream);
    
//private:
    HTMSubTreeNode*     createChildNode(HTMSubTreeNode* current, STARE_ArrayIndexSpatialValue code, int level);
    void                addSTAREID(STARE_ENCODE key);
    // Helper functions
    STARE_ENCODE                 getSTARELEVELCode(STARE_ENCODE key, int level);
    int                 rec_intersect(HTMSubTreeNode* root_a, HTMSubTreeNode* root_b, std::list<STARE_ENCODE> * result);
    HTMSubTreeNode*     getHighestRoot(HTMSubTreeNode* Ins_root);
    HTMSubTreeNode*     getPotentialBranch(HTMSubTreeNode* root_a, HTMSubTreeNode* root_b);
    void                tryGroupLeaves(HTMSubTreeNode* curNode, std::list<HTMSubTreeNode*> path);
};

void HTMSubTree_test_Create_Print();
void HTMSubTree_test_HighestRoot();
void HTMSubTree_test_GetPotentialBranch();
void HTMSubTree_test_Intersect();
void HTMSubTree_test_Create_Duplicate_Print();
void HTMSubTree_vs_SpatialRange_Create_small();
void HTMSubTree_vs_SpatialRange_Create_large();
void HTMSubTree_vs_SpatialRange_Create_MOD05_1();
void HTMSubTree_vs_SpatialRange_Create_MOD05_2();
void HTMSubTree_vs_SpatialRange_Create_VNP03DNB_1();
void HTMSubTree_vs_SpatialRange_Intersect_small();
void HTMSubTree_vs_SpatialRange_Intersect_large();
void HTMSubTree_vs_SpatialRange_Intersect_MOD05_1();
void HTMSubTree_vs_SpatialRange_Intersect_MOD05_2();
void HTMSubTree_vs_SpatialRange_Intersect_VNP03DNB_1();
void HTMSubTree_vs_SpatialRange_Intersect_MOD05_2_verify();
/* ========== <Timer class> =========== */
class timer
{

	std::chrono::system_clock::time_point startCounter;
	std::chrono::system_clock::time_point endCounter;
	double  result = 0.0;
	double  resultInSecond = 0.0;
	double  resultInMilliSecond = 0.0;
	double  resultInMicroSecond = 0.0;

public:
	timer();

	void start();

	void end();

	void reStart();


	void reStartValue(double value);


	double getResult();

	double getResultInSecond();

	double getResultInMilliSecond();

	double getResultInMicroSecond();
};


#endif /* INCLUDE_HTMSUBTREE_H_ */