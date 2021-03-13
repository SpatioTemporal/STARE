/*
  File: SkipList.C
  Author: Bruno Grossniklaus, 13.11.97
  Mod:    Gyorgy Fekete, Oct-09-2002
  Version: 1.0
  History:
  Nov-13-1997; Gro; Version 1.0
  Oct-09-2002; JHU; Version 1.1

*/

#include <iostream> // cout
#include <iomanip> // setw()
#include <stdlib.h> // rand(), drand48()


// #include "SkipListMacro.h" // NIL, SKIPLIST_MAXLEVEL, OS_DEFAULT_WIDTH 
#include "SkipListElement.h" 
#include "SkipList.h"


double drand48()
{
  double result;

#ifdef _WIN32
  result = (double) rand();
  result /= RAND_MAX;
#else
  result = (double) random();
  result /= LONG_MAX;
#endif
  return result;
}


////////////////////////////////////////////////////////////////////////////////
// get new element level using given probability
////////////////////////////////////////////////////////////////////////////////
long getNewLevel(long maxLevel, float probability)
{
  long newLevel = 0;
  
  while ( (newLevel < maxLevel - 1) && (drand48() < probability) ) { // fast hack. fix later
    newLevel++;
  }

  return(newLevel);
}

////////////////////////////////////////////////////////////////////////////////
SkipList::SkipList(float probability)
  : myProbability(probability)
{
  myHeader = new SkipListElement(); // get memory for header element
  myHeader->setKey( KEY_MAX);
  myLength = 0;
  iter = myHeader;
}

////////////////////////////////////////////////////////////////////////////////
SkipList::~SkipList()
{
  delete myHeader; // free memory for header element
}

////////////////////////////////////////////////////////////////////////////////
void SkipList::insert(const Key searchKey, const Value value)
{
	int i;
	long newLevel;
	SkipListElement* element;
	SkipListElement* nextElement;
	SkipListElement  update(SKIPLIST_MAXLEVEL);

	// scan all levels while key < searchKey
	// starting with header in his level
	element = myHeader;
	for(i=myHeader->getLevel(); i>=0; i--) {
		nextElement = element->getElement(i);
		while( (nextElement != NIL) && (nextElement->getKey() < searchKey) ) {
			element=nextElement;
			nextElement = element->getElement(i);
		}
		update.setElement(i, element); // save level pointer
	}

	element=element->getElement(0); // key is < searchKey

	// key exists. set new value TODO Note: NEW VALUE!!! Don't have multiple values at a key.
	//
	if( (element != NIL) && (element->getKey() == searchKey) ) {
		//    cerr << "(dup " << searchKey << ")"; // print # to cout. remove later!
		// TODO Note for htm value is zero, having the element in the skip list is significant.
		element->setValue(value);
	}  else {			// new key. add to list
		// get new level and fix list level
		newLevel = getNewLevel(SKIPLIST_MAXLEVEL, myProbability); // get new level
		if (newLevel > myHeader->getLevel() ) { // adjust header level
			for (i=myHeader->getLevel() + 1; i<=newLevel; i++) {
				update.setElement(i, myHeader); // adjust new pointer of new element
			}
			myHeader->setLevel(newLevel); // set new header level
		}
		// make new element [NEW *******]
		myLength++;
		element = new SkipListElement(newLevel, searchKey, value); // TODO Any corresponding delete?
		for (i=0; i<= newLevel; i++ ) { // scan all levels
			// set next pointer of new element
			element->setElement(i, update.getElement(i)->getElement(i));
			update.getElement(i)->setElement(i, element);
		}

		/*
    // fix level of element -- why not?
    if(newLevel < update.getLevel()) {
      update.setLevel(newLevel);
    }
		 */
	}
}

////////////////////////////////////////////////////////////////////////////////
// greatest key less than searchKey.. almost completely, but not
// quite entirely unlike a search ...
//
Key SkipList::findMAX(const Key searchKey) const
{
  int i;
  SkipListElement* element;
  SkipListElement* nextElement;
  Key retKey;

  element = myHeader;
  for(i=myHeader->getLevel(); i>=0; i--) {
    nextElement = element->getElement(i);
    while( (nextElement != NIL) && (nextElement->getKey() < searchKey) ) {
      element=nextElement;
      nextElement = element->getElement(i);
    }
  }
  // now nextElement is >= searchKey
  // and element is < searchKey
  // therefore elemnt  key is largest key less than current

  // if this were search:
  // element=element->getElement(0); // skip to >= 

  
  if(element != NIL) {
    retKey = element->getKey();
    return( retKey == KEY_MAX ? (KEY_MIN) : retKey);
  }
  else
    return((Key) KEY_MAX);
}

Value SkipList::searchAlt(const Key searchKey)
{
  int i;
  SkipListElement* element;
  SkipListElement* nextElement;

  element = myHeader;
  for(i=myHeader->getLevel(); i>=0; i--) {
    nextElement = element->getElement(i);
    while( (nextElement != NIL) && (nextElement->getKey() < searchKey) ) {
      element=nextElement;
      nextElement = element->getElement(i);
    }
  }

  element=element->getElement(0); // key is < searchKey

  // if key exists return value else ERROR
  if( (element != NIL) && (element->getKey() == searchKey) )
    return(element->getValue());
  else
    return(SKIPLIST_NOT_FOUND);
}
Key SkipList::findMIN(const Key searchKey) const
  // smallest greater than searchKey.. almost completely, but not
  // quite entirely unlike a search ...
{
  int i;
  SkipListElement* element     = NULL;
  SkipListElement* nextElement = NULL;
  Key retKey;

  element = myHeader;
  for(i=myHeader->getLevel(); i>=0; i--) {
    nextElement = element->getElement(i);
    while( (nextElement != NIL) && (nextElement->getKey() <= searchKey) ) {
      element=nextElement;
      nextElement = element->getElement(i);
    }
  }
  // now nextElement is > searchKey
  // element is <= , make it >
  //
  element = nextElement;
  if(element != NIL) {
    retKey = element->getKey();
    return( retKey == KEY_MAX ? (KEY_MIN) : retKey);
  }
  else
    return((Key) KEY_MAX);
}

Value SkipList::search(const Key searchKey, int iterator_flag)
{
  int i;
  SkipListElement* element;
  SkipListElement* nextElement;

  element = myHeader;
  for(i=myHeader->getLevel(); i>=0; i--) {
    nextElement = element->getElement(i);
    while( (nextElement != NIL) && (nextElement->getKey() < searchKey) ) {
      element=nextElement;
      nextElement = element->getElement(i);
    }
  }

  element=element->getElement(0); // key is < searchKey

  // if key exists return value else ERROR
  if( (element != NIL) && (element->getKey() == searchKey) ) {
    if (iterator_flag > 0){
      iter = element;
    }
    return(element->getValue());
  }
  else
    return(SKIPLIST_NOT_FOUND);
}
Value SkipList::search(const Key searchKey)
{
  int i;
  SkipListElement* element;
  SkipListElement* nextElement;

  element = myHeader;
  for(i=myHeader->getLevel(); i>=0; i--) {
    nextElement = element->getElement(i);
    while( (nextElement != NIL) && (nextElement->getKey() < searchKey) ) {
      element=nextElement;
      nextElement = element->getElement(i);
    }
  }

  element=element->getElement(0); // key is < searchKey

  // if key exists return value else ERROR
  if( (element != NIL) && (element->getKey() == searchKey) )
    return(element->getValue());
  else
    return(SKIPLIST_NOT_FOUND);
}

//////////ITERATOR/////////////////////////////////////////////////////////////
// see the .h file
///////////////////////////////////////////////////////////////////////////////
void SkipList::list(ostream & os)
{
  os << "Listing start *********************************************" << endl;
  SkipListElement* element;
  SkipListElement* nextElement;

  element = myHeader;
  nextElement = element->getElement(0);
  while( (nextElement != NIL) ) {
    /*    
    os << setw(20)
       << nextElement->getKey() << " " << setw(4)
       << nextElement->getValue() << " " <<  endl;
    */
#define FMTX(x) setw(16) << setfill('0') << hex << x << dec
    os << " 0x" << FMTX(nextElement->getKey())
       << " 0x" << FMTX(nextElement->getValue())
       << " " << setw(20) << nextElement->getValue()
       << endl << flush;
#undef FMTX
    
    element=nextElement;
    nextElement = element->getElement(0);
  }
  os << "List end **************************************************" << endl;
  return;
}

#if 0
////////////////////////////////////////////////////////////////////////////////
// List a pair of closely coupled lists.
//
void list2(ostream & os, SkipList& a, SkipList& b)
{
  os << "Listing start *********************************************" << endl;
  SkipListElement* a_element,b_element;
  SkipListElement* a_nextElement,b_nextElement;

  ETC.

  a_element = a.myHeader;
  nextElement = element->getElement(0);

  
  while( (nextElement != NIL) ) {
    /*    
    os << setw(20)
       << nextElement->getKey() << " " << setw(4)
       << nextElement->getValue() << " " <<  endl;
    */
#define FMTX(x) setw(16) << setfill('0') << hex << x << dec
    os << " 0x" << FMTX(a.nextElement->getKey())
       << " 0x" << FMTX(a.nextElement->getValue())
       << " 0x" << FMTX(b.nextElement->getKey())
       << " 0x" << FMTX(b.nextElement->getValue())
       << " " << setw(20) << b.nextElement->getValue()
       << endl << flush;
#undef FMTX
    
    element=nextElement;
    nextElement = element->getElement(0);
  }
  os << "List end **************************************************" << endl;
  return;
}
#endif


////////////////////////////////////////////////////////////////////////////////
void SkipList::freeRange(const Key loKey, const Key hiKey)
{
  /* Very similar to free, but frees a range of keys
     from lo to hi, inclusive */

  int i;
  SkipListElement* element;
  SkipListElement* nextElement;


  // scan all levels while key < searchKey
  // starting with header in his level
  //
  //  cerr << "freeRange from " << loKey << " to " << hiKey << endl;
  element = myHeader;
  for(i=myHeader->getLevel(); i>=0; i--) {
    nextElement = element->getElement(i);
    while( (nextElement != NIL) && (nextElement->getKey() < loKey) ) {
      element=nextElement;
      nextElement = element->getElement(i);
    }
  }

  element=element->getElement(0);  // key is < loKey

  // [ed:diff] while keys exists

  //  cerr << "freeRange begins deleting at  " << element->getKey() << endl;
  while( (element != NIL) && (element->getKey() <= hiKey) ) {
    //    cerr << "freeRange wants to delete " << element->getKey() << endl;
    nextElement = element->getElement(0);
    free(element->getKey());
    element = nextElement;
  }
}
////////////////////////////////////////////////////////////////////////////////
void SkipList::free(const Key searchKey)
{
	int i;
	SkipListElement* element;
	SkipListElement* nextElement;
	SkipListElement  update(SKIPLIST_MAXLEVEL);

	// scan all levels while key < searchKey
	// starting with header in his level
	element = myHeader;
	for(i=myHeader->getLevel(); i>=0; i--) {
		nextElement = element->getElement(i);
		while( (nextElement != NIL) && (nextElement->getKey() < searchKey) ) {
			element=nextElement;
			nextElement = element->getElement(i);
		}
		update.setElement(i, element); // save level pointer
	}

	element=element->getElement(0);  // key is < searchKey

	// if key exists
	if( (element != NIL) && (element->getKey() == searchKey) ) {
		for(i=0; i<=myHeader->getLevel(); i++) { // save next pointers
			if (update.getElement(i)->getElement(i) == element) {
				update.getElement(i)->setElement(i, element->getElement(i));
			}
		}

		delete (element); // free memory of element
		myLength--;

		// set new header level
		while ( (myHeader->getLevel() > 0) && (myHeader->getElement(myHeader->getLevel()) == NIL) ) {
			myHeader->setLevel(myHeader->getLevel()-1);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
void SkipList::freeAll()
{
  /* Very similar to free, but frees all */

  SkipListElement* element;
  SkipListElement* nextElement;

  element = myHeader;
  element=element->getElement(0);
  while( (element != NIL)  ) {
    nextElement = element->getElement(0);
    free(element->getKey());
    element = nextElement;
  }
}

////////////////////////////////////////////////////////////////////////////////
ostream& operator<<(ostream& os, const SkipList& list)
{
  int i;
  SkipListElement* element;

  // print header info
  element = list.myHeader;
  os << "Sl prob. = " << list.myProbability << endl;
  os << "Header " << setw(OS_DEFAULT_WIDTH) << "" << *element << endl;

  // print all levels
  //!!  for(i=SKIPLIST_MAXLEVEL - 1; i>=0; i--){
  for(i=list.myHeader->getLevel(); i>=0; i--){
    element = list.myHeader->getElement(i);
    if (element!=NIL) os << endl;
    while(element!=NIL) {
      // print element in level
      os << "(i" << setw(OS_DEFAULT_WIDTH) << i << " (elt " << *element << ")" << endl;
      element = element->getElement(i);
    }
  }
  return(os);
}

long SkipList::getCount() {
	long count = 0;
	SkipListElement* element;
	SkipListElement* nextElement;

	element = myHeader;
	nextElement = element->getElement(0);

	while( (nextElement != NIL) ) {
		count++;
		element=nextElement;
		nextElement = element->getElement(0);
	}
	return count;
}

//// STATISTICS on skiplist
void SkipList::stat()
{
  int count = 0;
  SkipListElement* element;
  SkipListElement* nextElement;

  element = myHeader;
  nextElement = element->getElement(0);

  while( (nextElement != NIL) ) {
    count++;
    element=nextElement;
    nextElement = element->getElement(0);
  }
  cout << "Have number of elements ... " << count << endl;
  cout << "Size  ..................... " << myLength << endl;
  {
    int *hist;
    hist = new int[20];
	int i;
    long totalPointers, usedPointers;
    for(i=0; i<20; i++){
      hist[i] = 0;
    }
    element = myHeader;
    count = 0;
    nextElement = element->getElement(0);
    while( (nextElement != NIL) ) {
      count++;
      hist[nextElement->getLevel()]++;
      element=nextElement;
      nextElement = element->getElement(0);
    }
    // 
    // There are SKIPLIST_MAXLEVEL * count available pointers
    //
    totalPointers = SKIPLIST_MAXLEVEL * count;
    usedPointers = 0;
    //
    // of these every node that is not at the max level wastes some
    //
    for(i=0; i<20; i++){
      if(hist[i] > 0) cout << setw(2) << i << ": " << setw(6) << hist[i] << endl;
      usedPointers += hist[i] * (1 + i);
    }
    cout << "Used  pointers " << usedPointers << endl;
    cout << "Total pointers " << totalPointers << " efficiency = " << 
      (double) usedPointers / (double) totalPointers << endl;
    delete hist;
  }
  return;
}
