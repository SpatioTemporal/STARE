//#     Filename:       sqlExample.cpp
//#
//#     Example code how to use sql interface
//#
//#
//#     Author:         Peter Z. Kunszt
//#
//#     Date:           Sept 6 2000
//#
//#
//#		Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#
//#     Modification History:
//#
//#     Oct 18, 2001 : Dennis C. Dinge -- Replaces ValVec with std::vector
//#
#include "SpatialIndex.h"
#include "SpatialConstraint.h"
#include "SpatialConvex.h"
#include "SpatialDomain.h"
#include "sqlInterface.h"

#ifndef __unix
// added by jim for HeapValidate
#include  <Windows.h>  
#include <malloc.h>
#endif

#include <stdio.h>
#include <vector>

#include	<time.h>

#define DBG_SHORT	1
#define DBG_MORE	2
#define DBG_FULL	3


int 
showtest(char *s, int n, int flag) {
  // display test results
  if (flag>0)	{
    if (n==0)	printf("%s: OK\n", s);
    else		printf("%s: %d FAIL\n",s,n);
  }
  return n;
}


int 
showlists(char *msg, int flag,
	  const ValueVectorUint64 & full, 
	  const ValueVectorUint64 & partial) {

  size_t i;

  if (flag>=DBG_MORE) 
    printf ("  (%s)\n",msg);
  if (flag>=DBG_MORE)
    printf ("    partial %ld \n", partial.size());
  if (flag==DBG_FULL)
    for ( i = 0; i < partial.size(); ++i)	
      printf ("      %I64d \n", partial[i]);
  if (flag>=DBG_MORE)
    printf ("    full %ld \n", full.size());
  if (flag==DBG_FULL)
    for ( i = 0; i < full.size(); ++i)
      printf ("      %I64d \n", full[i]);
  return 0;
}


// return 1 if failed, 0 otherwise
int
runtest1 (const SpatialIndex &si, double ra, double dec, uint64 htmid, int flag) {
  //--------------------------------------
  // test whether htmid/name is calculated correctly
  //--------------------------------------
  char name[80];
  uint64 result = si.idByPoint(ra,dec);
  si.nameById(result,name);
  if (flag==DBG_FULL) 
    printf ("%I64d %I64d %s\n", result, htmid, name);
  return !(result==htmid);
}

void
printRanges(char *msg, size_t nRanges, ValueVector &ranges, int flag) {
	
  if (flag>=DBG_MORE)
    cout << msg << endl;

  if (flag != DBG_FULL) return;

  for(size_t i = 0; i < nRanges; i++) {
    PRINTID(ranges[i].lo);
    printf(" - ");
    PRINTID(ranges[i].hi);
    printf("\n");
  }
}


int
test1 (int flag) {
  //----------------------------------------------
  // test1: the tree, the id and name functions
  //----------------------------------------------
  int nfailed = 0;
  SpatialIndex si (20,5);
  nfailed += runtest1(si, 265.71551323737202,	54.912678632977901, 14714254340846LL, flag);
  nfailed += runtest1(si, 263.68194109736601,	52.910098376617803,	14719411404987LL, flag);
  nfailed += runtest1(si, 262.32866485316401,	56.556375868102201,	14769733472977LL, flag);
  nfailed += runtest1(si, 265.92519883797502,	54.279783091712503,	14714393877464LL, flag);
  nfailed += runtest1(si, 258.13552143050498,	62.874434494575198,	14739546626805LL, flag);
  nfailed += runtest1(si, 352.98985216578001,	-1.0858822796771599, 12651686112216LL, flag);
  nfailed += runtest1(si, 262.47048108711101,	59.644675599175898,	14734908807505LL, flag);
  nfailed += runtest1(si, 157.57284354252599,	-0.63706065113772803, 10685911432503LL, flag);
  nfailed += runtest1(si, 157.57284354374499,	-0.63706072535913905, 10685911432503LL, flag);
  nfailed += runtest1(si, 355.18143830087803,	-0.82111306827669805, 12659569683335LL, flag);
  nfailed += runtest1(si, 4.9381359131341496,	0.36529753946729299, 17043525204641LL, flag);
  nfailed += runtest1(si, 357.34274224451298,	1.0137965490299601,	13197792939994LL, flag);
  nfailed += runtest1(si, 257.57792882693798,	55.839904542548098,	14771402519458LL, flag);
  nfailed += runtest1(si, 54.410099143093802,	1.1224984464672401,	16635140448235LL, flag);
  nfailed += runtest1(si, 263.31866405354202,	57.529972789376899,	14770033083494LL, flag);
  nfailed += runtest1(si, 153.73966881566901,	0.80726269051063804, 15550955221138LL, flag);
  nfailed += runtest1(si, 8.8870408419115794,	-0.50562880740024896, 8805219256649LL, flag);
  nfailed += runtest1(si, 56.191084275947702,	-0.77519680158875004, 9470641236751LL, flag);
  nfailed += runtest1(si, 49.922606963224801,	1.2090462251787299,	16632184037791LL, flag);

  return showtest("Test1 (tree)\t", nfailed, flag);
}


int
test2 (int flag) {
  //----------------------------------------------
  // test2: the SpatialConstraint class
  //----------------------------------------------
  int nfailed = 0;

  SpatialIndex si (6,3);
  SpatialVector sz (0.,90.);
  SpatialVector sx (0.,0.);
  SpatialVector sy (90.,0.);
  SpatialVector sw (0.,-90.);
  SpatialConstraint sc (sz, .9);

  nfailed += !sc.contains(sz);
  nfailed += sc.contains(sx);
  nfailed += sc.contains(sy);
  nfailed += sc.contains(sw);

  if (flag>=DBG_MORE) sc.write(cout);

  return showtest("Test2 (constr)\t", nfailed, flag);
}


int
test3 (int flag) {
  //----------------------------------------------
  // test3: the basic convex  tests
  //----------------------------------------------

  int nfailed = 0;
  ValueVectorUint64 full, partial;
  SpatialIndex si (3,3);
  SpatialVector sx (0.,0.);
  SpatialVector sz (0.,90.);
  SpatialConstraint sd (sx, .98);
  SpatialConstraint sc (sz, .98);
  SpatialConvex cc;

  // 1. add a single constraint along the z axis

  cc.add(sc);
  cc.intersect(&si, &partial, &full);

  if (flag>=DBG_MORE) cc.write(cout);
		
  nfailed += !(partial.size()==12);
  nfailed += !(full.size()==4);
  showlists("convex 1",flag,full,partial);

	// 2. add a second constraint to the convex along the x axis

  cc.add(sd);
  partial.clear();
  full.clear();
  cc.intersect(&si, &partial, &full);

  if (flag>=DBG_MORE) cc.write(cout);

  nfailed += !(partial.size()==0);
  nfailed += !(full.size()==0);
  showlists("convex 2",flag,full,partial);

  return showtest("Test3 (convex)\t", nfailed, flag);
}


int
test4 (int flag) {

  int nfailed = 0;
  ValueVectorUint64 full, partial;
  SpatialIndex  si (3,3);
  SpatialVector sx (0.,0.);
  SpatialVector sz (0.,90.);
  SpatialConstraint sd (sx, .98);
  SpatialConstraint sc (sz, .98);
  SpatialConvex cc;
  SpatialConvex cd;
  SpatialDomain sdm;

  // create two convexes, along x and z
  cc.add(sc);
  cd.add(sd);
	
  // add them to the domain
  sdm.add(cc);
  sdm.add(cd);

  sdm.intersect(&si, partial, full);

  if (flag>=DBG_MORE) sdm.write(cout);
	
  nfailed += !(partial.size()==24);
  nfailed += !(full.size()==8);
  showlists("domain 1",flag,full,partial);

  return showtest("Test4 (domain)\t",nfailed,flag);
}


int 
itest1(int flag)
{
  int n=0;
  MsgStr errMsg;
  htmSqlInterface htm(6);
  HTM_ID id;
  char lookup[100] = "J2000 6 41.4 47.9";

  /* 1st Example of id lookup (using your diagnostic syntax)*/

  errMsg = htm.lookupIDDiagnostic(lookup);
  if(errMsg.empty()) {
    id = htm.lookupID(lookup);
    n = (id != 65351);

    if (flag>= DBG_MORE) {
      printf("lookup \"%s\" => ",lookup);
      PRINTID(id);
      printf("\n");
    }
  } else
    cout << errMsg;

  return showtest("iTest1 (lookup#1)",n,flag);
}


int 
itest2(int flag)
{
  int n=0;
  MsgStr errMsg;
  htmSqlInterface htm(6);
  HTM_ID id;
  char lookup[100] = "J2000 6 41.4 47.9";

  /* 2nd Example of id lookup (using err)
     I like this better because you don't need
     2 calls and you don't need an intermediate errMsg string.
  */

  id = htm.lookupID(lookup);
  n = (id != 65351);

  if(htm.err())
    cout << htm.error() << endl;
  else {
    if (flag>= DBG_MORE) {
      printf("lookup \"%s\" => ",lookup);
      PRINTID(id);
      printf("\n");
    }
  }
  return showtest("iTest2 (lookup#2)",n,flag);
}


int 
itest3(int flag)
{
  int n=0;

  MsgStr errMsg;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm(6);
  char circle[100] = "J2000 6 41.4 47.9 2.0";

  /* 1st Example of circle region */

  errMsg = htm.circleRegionDiagnostic(circle);

  if(errMsg.empty()) {
    nRanges = htm.circleRegion(circle,ranges);
    printRanges(circle,nRanges,ranges,flag);
  } else
    cout << errMsg;

  n = (nRanges != 2);
  return showtest("iTest3 (circle#1)",n,flag);
}


int 
itest4(int flag)
{
  int n=0;
  MsgStr errMsg;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm(6);
  char circle[100] = "J2000 6 41.4 47.9 8.0";

  /* 2nd Example of circle region */

  nRanges = htm.circleRegion(circle,ranges);

  if(htm.err())
    cout << htm.error() << endl;
  else {
    printRanges(circle,nRanges,ranges,flag);
  }

  n = (nRanges != 2);
  return showtest("iTest4 (circle#2)",n,flag);
}


int 
itest5(int flag)
{
  int n=0;
  MsgStr errMsg;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm(6);
  char circle[100] = "J2000 6 41.4 47.9 2.0";

  /* Generic interface */

  strcpy(circle,"CIRCLE J2000 6 41.4 47.9 20");
  nRanges = htm.intersect2(circle, ranges);
  if(htm.err())
    cout << htm.error() << endl;
  else {
    printRanges(circle,nRanges,ranges,flag);
  }

  n = (nRanges != 4);
  return showtest("iTest5 (circle#3)",n,flag);
}


int 
itest6(int flag)
{
  int n=0;
  MsgStr errMsg;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm(6);
  char hull[100]	 = "J2000 6 41.4 47.9 41.2.47.9 41.0 47.5 41.4 48";

  /* Example of convex hull */

  nRanges = htm.convexHull(hull,ranges);
  if(htm.err())
    cout << htm.error() << endl;
  else
    printRanges(hull,nRanges,ranges,flag);

  n = (nRanges != 9);
  return showtest("iTest6 (cHull#1)",n,flag);
}


int 
itest7(int flag)
{
  int n=0;
  MsgStr errMsg;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm(6);
  char hull[100] = "CONVEX J2000 1 1 -1 1 -1 -1 1 -1"; // counterclockwise
  //	char hull[100] = "CONVEX J2000 1 1 1 -1 -1 -1 -1 1"; // clockwise
  //	char hull[100] = "CONVEX J2000 1 1 -1 -1 -1 1 1 -1"; // diag + cw
  //	char hull[100] = "CONVEX J2000 1 1 -1 -1 1 -1 -1 1"; // diag+ccw

  /* Generic interface */
  nRanges = htm.intersect2(hull, ranges);
  if(htm.err())
    cout << htm.error() << endl;
  else
    printRanges(hull,nRanges,ranges,flag);

  n = (nRanges != 9);
  n = 0;
  return showtest("iTest7 (cHull#2)",n,flag);
}


int 
itest8(int flag)
{
  int n=0;
  MsgStr errMsg;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm(6);
  char domain[100] = "DOMAIN 6 1 2 1.0 0.0 0.0 0.0 0.0 1.0 0.0 0.0";

  /* Example of domain */
  nRanges = htm.domain(domain,ranges);
  if(htm.err())
    cout << htm.error() << endl;
  else
    printRanges(domain, nRanges,ranges,flag);

  n = (nRanges != 2);
  return showtest("iTest8 (domain#1)",n,flag);
}


int 
itest9(int flag)
{
  int n=0;
  MsgStr errMsg;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm(6);

  char domain[100] ="DOMAIN 1 2 1.0 0.0 0.0 0.0 0.0 1.0 0.0 0.0";

  /* Generic interface */
  nRanges = htm.intersect2(domain, ranges);
  if(htm.err())
    cout << htm.error() << endl;
  else
    printRanges(domain, nRanges,ranges,flag);
		
  n = (nRanges != 2);
  return showtest("iTest9 (domain#2)",n,flag);
}

// added by jim to test my case
int 
itest10(int flag)
{
  int n=0;
  int64 result = 0;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm;

  char point[100] ="J2000 10 185 0 ";
  char circle[100] = "CIRCLE J2000 3 41.4 47.9 .1";
  char square[100] = "CONVEX J2000 6 175 -5 175 5 185 5 185 -5";
  /////////////////////////////////////////////////////////////////////
  result = htm.lookupID(point);	// do the lookup to get the HTM_ID
  if (htm.err()) {
    n++;
  }

  nRanges = htm.intersect1(circle,ranges);
  if ((htm.err()) | (nRanges > 500)) {
    n++;
  }									 


  return showtest("iTest10 (lookup and intersect circle)",n,flag);;//showtest("iTest10 (Point+CircleJim)",n,flag);
}

// added by jim to test my case
int 
itest11(int flag)
{
  int n=0;
  int64 result = 0;
  ValueVector ranges;
  size_t nRanges;
  htmSqlInterface htm;
  std::vector<int> listofint;

  char square[100] = "CONVEX J2000 6 175 -5 175 5 185 5 185 -5";
  /////////////////////////////////////////////////////////////////////
  // Test vector compatibility
  cerr << "List size " << listofint.size() << endl;
  listofint.resize(1);
  listofint[0] = 22;
  listofint.insert(listofint.end(), listofint[listofint.size() - 1]);
  cerr << "LIST[0] is " << listofint[0] << endl;
  cerr << "LIST[1] is " << listofint[1] << endl;
  cerr << "List size " << listofint.size() << endl;

  /////////////////////////////////////////////////////////////////////
  nRanges = htm.intersect1(square,ranges);
  if ((htm.err()) | (nRanges > 500)) {
    n++;
  }	

  return showtest("iTest10 (Jim XP_HTM SQL, intersect1 square)",n,flag);;//showtest("iTest10 (Point+CircleJim)",n,flag);
}


int 
mtest_lookup(int flag){
  int n=0;
  MsgStr errMsg;
  htmSqlInterface htm(6);
  HTM_ID id;
  long counter = 1L;
  char lookup[100] = "J2000 6 41.4 47.9";

  /* run  millions of times*/
  while(counter-- > 0){
    // artificial memory leak 
    // (void ) malloc (20);
    id = htm.lookupID(lookup);
    if(counter % 50000 == 0)
      cerr << "Going " << counter << endl;
  }
  return showtest("iTest1 (lookup#1)",n,flag);
}

#ifdef NEVER

extern "C" {
  void memleak();
  void ckp1();
  void ckp2();
  void s2s1();
}
#endif

void faster(char *spec){

  int64		result;	
  htmSqlInterface *htm;
  htm = new htmSqlInterface();
  char czErrorMsg[1000] = "none";
  result = htm->lookupID(spec);	// do the lookup to get the HTM_ID



  if (htm->err()) 
    {strcpy(czErrorMsg,htm->error());
    if (strlen(czErrorMsg) == 0)
      strcpy(czErrorMsg,"HTM code failed to give diagnostic message");
    }	// return error string if there is one.	

  delete htm;

}
extern  "C" { void cc_parseVectors(char *spec, int *level, double *ra, double *dec);  } ;

// extern cc_parseVectors(char *spec, int *level, double *ra, double *dec);

int
main(int argc, char *argv[]) {

  // debug level verbosity: 0-silent, 1-oneline, 2-more 3-full

  // char *s;
  int nfailed = 0;
  int dbglvl	= 1;
  HTM_ID id;
  char futyi[50] = "J2000 6 41.4 47.9";

	
  // s = (char *) malloc(1024);
	
  if(1){
    nfailed += test1(dbglvl);
    nfailed += test2(dbglvl);
    nfailed += test3(dbglvl);
    nfailed += test4(dbglvl);

    nfailed += itest1(dbglvl);
    nfailed += itest2(dbglvl);
    nfailed += itest3(dbglvl);
    nfailed += itest4(dbglvl);
    nfailed += itest5(dbglvl);
    nfailed += itest6(dbglvl);
    nfailed += itest7(dbglvl);
    nfailed += itest8(dbglvl);
    nfailed += itest9(dbglvl);
    nfailed += itest9(dbglvl);
    nfailed += itest10(dbglvl);
    nfailed += itest11(dbglvl);
  }

	
  if(1){		
    //ckp1();

    htmSqlInterface *htm = new htmSqlInterface(6);
				
    id = htm->lookupID(futyi);
    delete htm;
    nfailed += mtest_lookup(dbglvl);
    //ckp2();
    //s2s1();

  }

  {
    time_t timer;
    time_t now, then;
    long initial = 10000L;
    long counter = initial;
    htmSqlInterface *htm = new htmSqlInterface(6);


    now = time( &timer );


    while(counter-- > 0){
      //			if ((counter % 10000) == 0)
      //				cerr << "Here: " << counter << endl;
      id = htm->lookupID(futyi);
    }
    then = time (&timer);
    cout << initial << " iterations in " << then - now << " seconds" << endl;
    cout << initial / (double) (then - now) << " per second" << endl;
    cout << (1000.0 * (then - now)) /  initial << " milliseconds each" << endl;
  }
  showtest("All tests", nfailed, 1);

  {
    double ra, dec;
    int level;
    cc_parseVectors(futyi, &level, &ra, &dec);
    cc_radec2ID(ra, dec);
    cout << "Futyi: (" << level << ") " << ra << ", " << dec << endl;

  }
  return 0;
}
