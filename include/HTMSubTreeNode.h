
#ifndef INCLUDE_HTMSUBTREENODE_H_
#define INCLUDE_HTMSUBTREENODE_H_Node

typedef int64 Key;      // STARE Encode - 64 bits    
typedef int64 STARE_ENCODE;     

int MAX_NUM_CHILD = 8;  // 8 children in the first level, and 4 children in other levels.
int MAX_NUM_CHILD_II = 4;

class HTMSubTreeNodeEntry{
public:
    HTMSubTreeNodeEntry();
    HTMSubTreeNodeEntry(Key _key);
    ~HTMSubTreeNodeEntry();
    HTMSubTreeNode* child;
    Key key;            // Key is stored twice (in the current node and in its entries), 
                        // since its will be easier in 
                        // travesing the subtree (one step is reduced).
}

class HTMSubTreeNode{
public:
    // Contructors
    HTMSubTreeNode();    
    HTMSubTreeNode(Key _key, bool _isLeaf, int _level, int _count)

    // Destructors
    ~HTMSubTreeNode();

    // Attributes
    Key key;            // STARE Encode
    int level;          // 0 <= level <= 27, (0 is the highest level)
    bool isLeaf;        // True or False
    int count;          // Total number of children
    HTMSubTreeNodeEntry* entries[MAX_NUM_CHILD];   // List of children if it is not a leaf
    
    // Methods


private:
}
#endif /* INCLUDE_HTMSUBTREENODE_H_ */