#ifndef _SkipListElement_H
#define _SkipListElement_H

#include <iostream>

/*
  File: SkipListElement.h
  Interface for skip list elements
  See William Pugh's paper: 
    Skip Lists: A Probabilistic Alternative to Balanced Trees
  Author: Bruno Grossniklaus, 13.11.97
  Version: 1.0
  History:
  13.11.97; Gro; Version 1.0
*/

#include <SpatialGeneral.h>

#define SKIPLIST_MAXLEVEL 6 // maximum node level
#define OS_DEFAULT_WIDTH 3  // width for keys, values for output
#define OS_INDEX_WIDTH 2    // width for index
#define NIL 0               // invalid pointer

#ifdef _WIN32
#define KEY_MAX _I64_MAX
#else
#if __cplusplus >= 201103L
#define KEY_MAX LLONG_MAX
#define KEY_MIN -KEY_MAX
//#define KEY_MAX ULLONG_MAX
//#define KEY_MIN 0
#else
#define KEY_MAX LONG_LONG_MAX
#endif
#endif

typedef int64 Key;            // key type TODO Why not uint64?
// typedef uint64 Key;            // key type TODO Why not uint64?
// typedef int Value;          // value type
typedef int64 Value; // mlr...

// class ostream;
class SkipListElement;

using namespace std;

class LINKAGE SkipListElement{
public:
  SkipListElement(long level = 0, Key  key = 0, Value  value = 0);
  ~SkipListElement();

  Key getKey() const {return(myKey);}; // get key of element
  void setKey(Key key) {myKey=key;}; // set key of element
  
  Value getValue() const {return(myValue);}; // get value of element
  void setValue(Value value) {myValue=value;}; // set value of element

  long getLevel() const {return(myLevel);}; // get level of element
  void setLevel(long level);
  // static void SkipListElement::prmatrix();
  static void prmatrix();
  SkipListElement* getElement(long level); // get next element in level
  void setElement(long level, SkipListElement* element); // set next element in level
  
  friend ostream& operator<<(ostream& os, const SkipListElement& element);

// private:
  long myLevel; // level of element
  Key myKey; // key of element
  Value myValue; // value of element
  SkipListElement* myNext[SKIPLIST_MAXLEVEL]; // pointers to next elements // TODO How to finalize?
};
#endif // _SkipListElement_H
