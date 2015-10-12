//#     Filename:       test1.cpp
//#
//#     Basic tests of the SpatialIndex classes
//#
//#     Author:         Dennis Dinge & Alex Szalay
//#
//#     Date:           Oct 18, 2001
//#
//#		Copyright (C) 2000  Dennis C. Dinge, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#
//#
//#

#include "SpatialIndex.h"
#include "SpatialConstraint.h"
#include "RangeConvex.h"
#include "SpatialDomain.h"

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


int
test1 (int flag) {
//----------------------------------------------
// test1: the tree, the id and name functions
//----------------------------------------------
	int nfailed = 0;

	SpatialIndex si (20,5);

	nfailed += runtest1(si, 265.71551323737202,	54.912678632977901, 14714254340846, flag);
	nfailed += runtest1(si, 263.68194109736601,	52.910098376617803,	14719411404987, flag);
	nfailed += runtest1(si, 262.32866485316401,	56.556375868102201,	14769733472977, flag);
	nfailed += runtest1(si, 265.92519883797502,	54.279783091712503,	14714393877464, flag);
	nfailed += runtest1(si, 258.13552143050498,	62.874434494575198,	14739546626805, flag);
	nfailed += runtest1(si, 352.98985216578001,	-1.0858822796771599, 12651686112216, flag);
	nfailed += runtest1(si, 262.47048108711101,	59.644675599175898,	14734908807505, flag);
	nfailed += runtest1(si, 157.57284354252599,	-0.63706065113772803, 10685911432503, flag);
	nfailed += runtest1(si, 157.57284354374499,	-0.63706072535913905, 10685911432503, flag);
	nfailed += runtest1(si, 355.18143830087803,	-0.82111306827669805, 12659569683335, flag);
	nfailed += runtest1(si, 4.9381359131341496,	0.36529753946729299, 17043525204641, flag);
	nfailed += runtest1(si, 357.34274224451298,	1.0137965490299601,	13197792939994, flag);
	nfailed += runtest1(si, 257.57792882693798,	55.839904542548098,	14771402519458, flag);
	nfailed += runtest1(si, 54.410099143093802,	1.1224984464672401,	16635140448235, flag);
	nfailed += runtest1(si, 263.31866405354202,	57.529972789376899,	14770033083494, flag);
	nfailed += runtest1(si, 153.73966881566901,	0.80726269051063804, 15550955221138, flag);
	nfailed += runtest1(si, 8.8870408419115794,	-0.50562880740024896, 8805219256649, flag);
	nfailed += runtest1(si, 56.191084275947702,	-0.77519680158875004, 9470641236751, flag);
	nfailed += runtest1(si, 49.922606963224801,	1.2090462251787299,	16632184037791, flag);

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
	RangeConvex cc;

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
	RangeConvex cc;
	RangeConvex cd;
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
main(int argc, char *argv[]) { 	

	
	int  dbglev  =2;	

	int  nfailed =0;
	
	// debug level verbosity: 0-silent, 1-oneline, 2-more 3-full
	nfailed += test1(dbglev);
	nfailed += test2(dbglev);
	nfailed += test3(dbglev);
	nfailed += test4(dbglev);

	showtest("-------------\nAll tests", nfailed, 1);
    return 0;
} 

