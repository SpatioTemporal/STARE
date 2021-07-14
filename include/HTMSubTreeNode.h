
#ifndef INCLUDE_HTMSUBTREENODE_H
#define INCLUDE_HTMSUBTREENODE_H
#include <stdint.h>
#include <cstddef>
#include <list>
#include <iostream>
#include <string>
#include <vector>

//TODO: These types should be combined with STARE.h 
//  (i.e., Key == STARE_ENCODE == STARE_ArrayIndexSpatialValue)
//  int64 and uint64 are defined in SpatialGeneral.h
//typedef unsigned long long Key;      // STARE Encode - 64 bits    
typedef unsigned long long STARE_ENCODE;     

int const MAX_NUM_CHILD = 8;  // 8 children in the first level, and 4 children in other levels.
int const MAX_NUM_CHILD_II = 4;

/*class HTMSubTreeNodeEntry{
public:
    HTMSubTreeNodeEntry();
    HTMSubTreeNodeEntry(Key _key);
    ~HTMSubTreeNodeEntry();
    HTMSubTreeNode* child;
    Key key;            // Key is stored twice (in the current node and in its entries), 
                        // since its will be easier in 
                        // travesing the subtree (one step is reduced).
};*/

class HTMSubTreeNode{
public:
    // Contructors
    HTMSubTreeNode();    
    HTMSubTreeNode(STARE_ENCODE _key, bool _isLeaf, int _level, int _count);

    // Destructors
    ~HTMSubTreeNode();

    // Attributes
    STARE_ENCODE key;            // STARE Encode
    int level;          // 0 <= level <= 27, (0 is the highest level)
    bool isLeaf;        // True or False
    int count;          // Total number of children
    //HTMSubTreeNodeEntry* entries[MAX_NUM_CHILD];   // List of children if it is not a leaf
    
    //entries
    HTMSubTreeNode* children[MAX_NUM_CHILD];
    STARE_ENCODE keys[MAX_NUM_CHILD];
    // Methods


private:
    STARE_ENCODE genSTARELEVELCode(STARE_ENCODE key, unsigned int level, unsigned int pos);
};
#endif /* INCLUDE_HTMSUBTREENODE_H_ */