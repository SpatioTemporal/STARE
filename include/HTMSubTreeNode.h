
#ifndef INCLUDE_HTMSUBTREENODE_H
#define INCLUDE_HTMSUBTREENODE_H
#include <stdint.h>
#include <cstddef>
#include <list>
#include <iostream>
#include <string>
#include <vector>
//typedef unsigned long long uint64;

typedef unsigned long long Key;      // STARE Encode - 64 bits    
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
    HTMSubTreeNode(Key _key, bool _isLeaf, int _level, int _count);

    // Destructors
    ~HTMSubTreeNode();

    // Attributes
    Key key;            // STARE Encode
    int level;          // 0 <= level <= 27, (0 is the highest level)
    bool isLeaf;        // True or False
    int count;          // Total number of children
    //HTMSubTreeNodeEntry* entries[MAX_NUM_CHILD];   // List of children if it is not a leaf
    
    //entries
    HTMSubTreeNode* children[MAX_NUM_CHILD];
    Key keys[MAX_NUM_CHILD];
    // Methods


private:
    Key genSTARELEVELCode(Key key, unsigned int level, unsigned int pos);
};
#endif /* INCLUDE_HTMSUBTREENODE_H_ */