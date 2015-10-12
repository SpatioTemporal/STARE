#include <vector.h>

#define LINKAGE

// class LINKAGE BitListIterator;

#include "SpatialIndex.h"

#include "SpatialConstraint.h"
//#include "SpatialConvex.h"
//#include "SpatialDomain.h"
#include "sqlInterface.h"

//  #ifndef __unix
//  // added by jim for HeapValidate
//  #include  <Windows.h>  
//  #include <malloc.h>
//  #endif




#include	<time.h>

#define DBG_SHORT	1
#define DBG_MORE	2
#define DBG_FULL	3

int main(int argc, char *argv[]) {

  // debug level verbosity: 0-silent, 1-oneline, 2-more 3-full

  // char *s;
  int nfailed = 0;
  int dbglvl	= 1;
  HTM_ID id;
  char futyi[50] = "J2000 6 41.4 47.9";

  if(1){		
    //ckp1();

    htmSqlInterface *htm = new htmSqlInterface(6);
				
    id = htm->lookupID(futyi);
    cout << "Done: " << id << endl;
    delete htm;
    // nfailed += mtest_lookup(dbglvl);
    //ckp2();
    // s2s1();

  }
  return 0;
}
