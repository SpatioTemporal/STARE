
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"

#include "HstmIndex.h"

// TODO #include the headers for the code you want to test
//#include "SpatialGeneral.h"
#include "SpatialException.h"
#include "SpatialIndex.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"

#include "HtmRangeIterator.h"
#include "BitShiftNameEncoding.h"
#include "EmbeddedLevelNameEncoding.h"

// TODO Add your test functions

double square(SpatialVector x){	return x*x; }
double norm(SpatialVector x){ return sqrt(square(x)); }
#define ASSERT_EQUALDM(msg,a,b,tol) ASSERT_LESSM(msg,norm(a-b),tol);
#define ASSERT_NOTEQUALDM(msg,a,b,tol) ASSERT_LESSM(msg,tol,norm(a-b));

#define componentCheck(xxx) 	{cout << setprecision(17); cout << "  xxx: " << xxx << endl << flush; cout << "v.xxx: " << v_.xxx() << endl << flush; cout << "delta: " << xxx - v_.xxx() << endl << flush;}

void precisionTest(){
	// Check that we understand the precision of the variables.
	ASSERT_EQUALDM(
			"SpatialVectors equal: ",
			SpatialVector(1.,1.,1.),
			SpatialVector(1.,1.,1.+1.0e-16),
			1.e-16);
	ASSERT_NOTEQUALDM(
			"SpatialVectors not equal: ",
			SpatialVector(1.,1.,1.),
			SpatialVector(1.,1.,1.+1.0e-15),
			1.e-16);
}

void lookupID() {
	int level          = 4;
	int saveLevel      = 5;
	htmInterface *htm_ = new htmInterface(level,saveLevel);
	float64 x =  0.50350931157595497;
	float64 y = -0.6109330459045055;
	float64 z = -0.6109330459045055;

	int id_ = htm_ ->lookupID(x,y,z);
	ASSERT_EQUALM("id from lookupID(x,y,z)",id_,3070);

	char name_[1024];
	htm_->index().nameById(id_,name_);
	//cout << "name_: '" << name_ << "'" << endl << flush;
	ASSERT_EQUALM("checking nameByID",name_,"S33332");
}

// TODO I just noticed there is an issue in dealing with parsing the IDs via nameByID -- I don't think they are interpreting the depth correctly. The index is constructed with a depth.  What happens if you give it an HTM string or ID that is associated with another depth?



void lookupOnTwoSaveLevels() {

	float64 x =  0.50350931157595497;
	float64 y = -0.6109330459045055;
	float64 z = -0.6109330459045055;

	int level          = 4;
	int saveLevel      = 5;
	htmInterface *htm0_ = new htmInterface(level,saveLevel);
	int id0 = htm0_ ->lookupID(x,y,z);

	level          = 4;
	saveLevel      = 2;
	htmInterface *htm1_ = new htmInterface(level,saveLevel);
	int id1 = htm1_ ->lookupID(x,y,z);

	ASSERT_EQUALM("id from 2 saveLevels: ",id0,id1);

}

#define ASSERT_EQUALM_NAMEBYID_(msg,expected,index,id,name){char *n; n=index.nameById(id,name); ASSERT_EQUALM(msg,expected,name);}
#define INDEX_(name){htm[levelOfName(name)]->index();}
#define PRINT_ID(msg,htm,name) {	SpatialIndex index = htm[levelOfName(name)]->index(); cout << msg << " indexLevel: " << index.getMaxlevel() << " id: " << flush; cout << index.idByName(name) << " name: " << name << " nameLevel: " << levelOfName(name); SpatialVector v; index.pointById(v,index.idByName(name)); cout << " v: " << v << endl << flush;}

void lookupOnMultipleLevels() {

	htmInterface *htm[6];
	for(int layer=1;layer<6;layer++){
		htm[layer] = new htmInterface(layer,8);
	}
	htm[0]=htm[1];

	float64 x =  0.50350931157595497;
	float64 y = -0.6109330459045055;
	float64 z = -0.6109330459045055;

	SpatialIndex index0_, index1_;
	int id0, id1;
	char name0[1024], name1[1024];

	int level          = 3;
	int saveLevel      = 2;

	index0_ = htm[level]->index();
	id0 =     htm[level]->lookupID(x,y,z);

	ASSERT_EQUALM("S3333: id0==767: ",767,id0);

	level          = 4;
	saveLevel      = 5;
	index1_ = htm[level]->index();
	id1 =     htm[level]->lookupID(x,y,z);

	ASSERT_EQUALM("S33332: id0==3070: ",3070,id1);

	ASSERT_EQUALM_NAMEBYID_("index0, name0","S3333",index0_,id0,name0);
	ASSERT_EQUALM_NAMEBYID_("index0, name1","S33332",index0_,id1,name1);
	ASSERT_EQUALM_NAMEBYID_("index1, name0","S3333",index1_,id0,name0);
	ASSERT_EQUALM_NAMEBYID_("index1, name1","S33332",index1_,id1,name1);

	//	PRINT_ID("index S33",INDEX_("S33"),"S33");
	//	PRINT_ID("index S33",INDEX_("S333"),"S333");
	//	PRINT_ID("index S33",INDEX_("S3332"),"S3332");

	/*
	PRINT_ID("098 index0",htm,"S33");
	PRINT_ID("099 index0",htm,"S333");
	PRINT_ID("100 index0",htm,"S3333");
	PRINT_ID("102 index0",htm,"S33332");
	 */

	/*
	level = 0;
	level = levelOfName("S00");
//	cout << hex;
	SpatialIndex index = htm[level]->index();
	for(int i = 1; i < 38; i++) {
		cout << "i: " << i << " " ;
		cout << "l: " << level << " " ;
		int idx = index.indexAtNodeIndex(i);
		cout << "idx; " << idx << " ";
		int id =  index.idAtNodeIndex(i); // nodes_[i].id_;
		cout << hex << "id: x" << id << " " << dec;
		char *n = new char[1024];
		index.nameById(id,n);
		cout << "n: " << n << " ";
		cout << hex << "ibn: x" << index.idByName(n) << " " << dec;
		cout << "l(n): " << levelOfName(n) << " ";
		cout << "leafN: " << index.leafNumberById(id) << " " ;
		cout << endl << flush;
	}

	cout << "index.layers_.size(): " << index.layersSize() << endl << flush;
	for(int i=0; i < index.layersSize(); i++){
//	for(int i=0; i < level+1; i++){
		cout << "i: " << i << " ";
		cout << "l: " << level << " " ;
		cout << "fIndex: " << index.firstIndexOfLayerAtLevel(level) << " ";
		cout << endl << flush;
	}
	 */
	char cTmp[1024];
	ASSERT_EQUALM("id0 to name to index1's id1",id0,index1_.idByName(index0_.nameById(id0,cTmp)));
	ASSERT_EQUALM("id1 to name to index0's id0",id1,index0_.idByName(index1_.nameById(id1,cTmp)));
}

void idReallyDeep() {
	uint32 level = 27;
	//	level = 1;
	uint32 shift = level*2+3; // GOTCHA IN SWITCH TO LEVEL
	uint64 htmID0 = 1;
	htmID0 = htmID0 << shift;
	char expected[1024];
	for(int i=0;i<1024;i++)expected[i] = 0;
	expected[0] = 'S';
	uint64 htmID=2;
	for(int i=1; i<level+2; i++) { // GOTCHA IN SWITCH TO LEVEL
		uint64 triangle = rand() % 4;
		//		triangle = 0;
		expected[i] = '0' + triangle;
		htmID = htmID << 2;
		htmID += triangle;
		//		cout << " " << i << " " << triangle << " " << expected[i] << " " << expected << " " << hex << htmID << dec << endl << flush;
	}
	expected[level+2]=0;// GOTCHA IN SWITCH TO LEVEL
	if(false){
		cout << " htmID0 " << htmID0 << " " << hex << htmID0 << dec << endl << flush;
		cout << " htmID  " << htmID << " " << hex << htmID << dec << endl << flush;
		cout << " expec  " << expected << endl << flush;
	}
	htmInterface *htm = new htmInterface(level,5);
	SpatialIndex index = htm->index();
	char foundName[1024]; for(int i=0;i<1024;i++)foundName[i] = 0;
	try {
		index.nameById(htmID,foundName);
	} catch (const SpatialException & e) {
		cout << "Exception " << e.what() << " n: " << foundName << endl << flush;
	}
	if(false) {
		cout << " fName  " << foundName << endl << flush;
		cout << " level " << level << endl << flush;
		cout << " htmID " << htmID << endl << flush;

	}
	ASSERT_EQUALM("level:    ",level,levelOfName(foundName));
	ASSERT_EQUALM("fName:    ",expected,foundName);
	ASSERT_EQUALM("htmID^-1: ",index.idByName(foundName),htmID);

	index.setMaxlevel(levelOfId(htmID));
	/*
	cout << " levelHtmId: " << levelOfId(htmID) << endl << flush;
	cout << hex;
	cout << " htmID0:    " << htmID0 << endl << flush;
	cout << " htmID:     " << htmID << endl << flush;
	cout << " nodeIndex: " << index.nodeIndexFromId(htmID) << endl << flush;
	cout << dec;
	 */
#define IOFFSET 9
	ASSERT_EQUALM("nodeIndex: ",(htmID&(~htmID0))+IOFFSET,index.nodeIndexFromId(htmID));
}
#undef ASSERT_EQUALM_NAMEBYID_

void pointById(){
	int level          = 4;
	int saveLevel      = 5;
	htmInterface *htm_ = new htmInterface(level,saveLevel);
	float64 x = 0.50350942389316267;
	float64 y = -0.61093299962057024;
	float64 z = -0.61093299962057024;
	double tolerance = 1.0e-14;
	int id_ = htm_->lookupID(x,y,z);
	SpatialVector v_;
	//	htm_->pointById(v_,id_);
	htm_->pointByHtmId(v_,id_);
	//	ASSERT_EQUALM("SpatialVectors x: ",0,2.0*(v_.x()-x)/abs(v_.x()+x));
	//	componentCheck(z);

	cout << hex;
	ASSERT_LESSM("SpatialVectors x: ",abs(x-v_.x()),tolerance);
	ASSERT_EQUALDM("SpatialVectors: ",SpatialVector(x,y,z),v_,tolerance);
	cout << dec;
}

void idByPoint1() {
	int level          = 8;
	int saveLevel      = 5;
	htmInterface *htm_ = new htmInterface(level,saveLevel);
	float64 x = 0.50350942389316267;
	float64 y = -0.61093299962057024;
	float64 z = -0.61093299962057024;
	int id_ = htm_->lookupID(x,y,z);

//	cout << "id_ " << id_ << endl << flush;

	const SpatialIndex index = htm_->index();

	SpatialVector v = SpatialVector(x,y,z);
	int idTest = index.idByPoint(v);

//	cout << "idT " << idTest << endl << flush;

	ASSERT_EQUALM("idByPoint1",id_,idTest);
}

/**
 * Try an example found in the source's comments.
 */
 void idByName() {
	int level         = 4;
	int saveLevel     = 5;
	htmInterface *htm_ = new htmInterface(level,saveLevel);
	const SpatialIndex index = htm_->index();
	const char *htmName = "N012023";
	uint64 htmID = index.idByName(htmName);
	uint64 htmIDExpected = 12683;
	ASSERT_EQUALM("N012023 == 12683?",htmIDExpected,htmID);
	ASSERT_EQUALM("level(N012023) == 5",5,levelOfName(htmName));
	ASSERT_EQUALM("level(12683)   == 5",5,levelOfId(htmID));

	htm_->changeLevel(levelOfId(htmID),saveLevel);
	const SpatialIndex index1 = htm_->index(); // Update the index.
	int htmIndexLayerSize = index1.layersSize();
	ASSERT_EQUALM("htm_ index number of levels==level+1==6",min(levelOfId(htmID)+1,saveLevel+1),htmIndexLayerSize);

	uint64 nodeIndex = index1.nodeIndexFromId(htmID);
	uint64 nodeIndexExpected = 4491 + 9; // for N012023
	ASSERT_EQUALM("nodeIndex(12683) == 4491+IOFFSET == 4500?",nodeIndexExpected,nodeIndex);
}

/**
 * Verify the symbolic name encoding using bit shifting.
 */
void checkBitShiftNameEncoding0() {
	BitShiftNameEncoding bitShiftName = BitShiftNameEncoding();

	uint htmId = 12683;
	string found = "'"+string(bitShiftName.nameById(htmId))+"'";
	ASSERT_EQUALM("N012023 == 12683?","'N012023'",found);
	ASSERT_EQUALM("12683 == N012023?",12683,bitShiftName.idByName("N012023"));
	ASSERT_EQUALM("level(N012023)",5,bitShiftName.levelById(htmId));

	uint64 testId = 0;
	string failureMessage = "'";
	try {
		testId = bitShiftName.idByName("N012024");
	} catch (SpatialFailure failure) {
		failureMessage += failure.what();
	}
	failureMessage += "'";
	ASSERT_EQUALM("INVALID DIGIT EXCEPTION","'BitShiftName:idByName-InvalidDigit'",failureMessage);

	bitShiftName.setId(12683);
	ASSERT_EQUALM("Set ID to 12683.",string("N012023"),bitShiftName.getName());

	bitShiftName.setName("N012022");
	ASSERT_EQUALM("Set name to N012022",12682,bitShiftName.getId());


	BitShiftNameEncoding *htm = new BitShiftNameEncoding("N012023");
	ASSERT_EQUALM("Construct using name.",12683,htm->getId());
	try {
		htm = new BitShiftNameEncoding("N012823");
	} catch (SpatialFailure failure) {
		ASSERT_EQUALM("Bad construction","BitShiftName:idByName-InvalidDigit",failure.what());
	}
	htm->setName("N012022");
	ASSERT_EQUALM("Construct using name.",12682,htm->getId());
	ASSERT_EQUALM("ID valid",true,htm->valid());
	htm = new BitShiftNameEncoding();
	ASSERT_EQUALM("ID invalid",false,htm->valid());

	htm->setName("N012022");
	ASSERT_EQUALM("Test bareID 12682 -> x318a vs. 4490 -> x118a",4490,htm->bareId());

	char tmp_buf[256];
	strcpy(tmp_buf,htm->getName());
	strcat(tmp_buf, " ");
	htm->setName("N012023");
	strcat(tmp_buf,htm->getName());
	ASSERT_EQUALM("Char[] construction test","N012022 N012023",tmp_buf);
}

/**
 * Test symbolic htm representation.
 */
void testRange() {
	HtmRange *htmR = new HtmRange;
	htmR->addRange(12682,12683);
	htmR->setEncoding(new BitShiftNameEncoding());
	htmR->setSymbolic(true);
	cout << dec;
	cout << "htmR expecting 'N012022 N012023', found: " << (*htmR) << endl << flush;
	cout << "htmR expecting 'N012023', found: "; htmR->print(0,cout,true); // Highs
	cout << "htmR expecting 'N012022', found: "; htmR->print(1,cout,true); // Lows
}

void testRangeContains() {
	HtmRange *h1 = new HtmRange;
	h1->purge();
	h1->addRange(50,100);
//	cout << "10..40 in 50..100? "<< h1->contains(10,40) << endl << flush;
//	h1->addRange(1,5);
//	h1->addRange(200,400);

//	cout << "10..40 in 50..100 " << endl << h1->contains(10,40) << endl << flush;
//	cout << "10..60 in 50..100 " << endl << h1->contains(10,60) << endl << flush;
//	cout << "10..110 in 50..100 " << endl << h1->contains(10,110) << endl << flush;
//	cout << "100.5..110 in 50..100 " << endl << h1->contains(100.5,110) << endl << flush;
//	cout << "60..110 in 50..100 " << endl << h1->contains(60,110) << endl << flush;
//	cout << "60..80 in 50..100 " << endl << h1->contains(60,80) << endl << flush;

	ASSERT_EQUALM("10..40 in 50..100",0,h1->contains(10,40));
	ASSERT_EQUALM("10..60 in 50..100",-1,h1->contains(10,60));
	ASSERT_EQUALM("10..110 in 50..100",-1,h1->contains(10,110));
	ASSERT_EQUALM("100.5..110 in 50..100",0,h1->contains(101,110));
	ASSERT_EQUALM("60..110 in 50..100",-1,h1->contains(60,110));
	ASSERT_EQUALM("60..80 in 50..100",1,h1->contains(60,80));

	h1->addRange(1,5);

	ASSERT_EQUALM("2: 10..40 in 50..100",0,h1->contains(10,40));
	ASSERT_EQUALM("2: 10..60 in 50..100",-1,h1->contains(10,60));
	ASSERT_EQUALM("2: 10..110 in 50..100",-1,h1->contains(10,110));
	ASSERT_EQUALM("2: 101..110 in 50..100",0,h1->contains(101,110));
	ASSERT_EQUALM("2: 60..110 in 50..100",-1,h1->contains(60,110));
	ASSERT_EQUALM("2: 60..80 in 50..100",1,h1->contains(60,80));


	h1->addRange(200,400);

	ASSERT_EQUALM("3: 10..40 in 50..100",0,h1->contains(10,40));
	ASSERT_EQUALM("3: 10..60 in 50..100",-1,h1->contains(10,60));
	ASSERT_EQUALM("3: 10..110 in 50..100",-1,h1->contains(10,110));
	ASSERT_EQUALM("3: 101..110 in 50..100",0,h1->contains(101,110));
	ASSERT_EQUALM("3: 60..110 in 50..100",-1,h1->contains(60,110));
	ASSERT_EQUALM("3: 60..80 in 50..100",1,h1->contains(60,80));

	ASSERT_EQUALM("3: 50..100 in 50..100",1,h1->contains(50,100));
	ASSERT_EQUALM("3: 50..100 in 50..100",1,h1->contains(50,100));
	ASSERT_EQUALM("3: 5..50 in 50..100",-1,h1->contains(5,50));
	ASSERT_EQUALM("3: 100..101 in 50..100",-1,h1->contains(100,101));

	h1->addRange(150,150);

	ASSERT_EQUALM("4: 150..150 in 150..150",1,h1->contains(150,150)); // From a bug found 2016-0331. MLR
	ASSERT_EQUALM("4: 75..75 in 50..100",1,h1->contains(75,75));
	ASSERT_EQUALM("4: 100..100 in 50.100",1, h1->contains(100,100));
	ASSERT_EQUALM("4: 50..50 IN 50..100", 1, h1->contains(50,50));
}

void testLatLonDegrees() {
	SpatialVector v;
	float64 a0=-1, a1=-2;
	ASSERT_EQUALM("LatLonFalse",false,v.getLatLonDegrees(a0,a1));

	v.setLatLonDegrees(30.0,45.0);
	ASSERT_EQUALM("LatLonTrue",true,v.getLatLonDegrees(a0,a1));
	ASSERT_EQUALM("a0==30",30,a0);
	ASSERT_EQUALM("a1==45",45,a1);

	v.setLatLonDegrees(-15.0,-27.0);
	ASSERT_EQUALM("LatLonTrue",true,v.getLatLonDegrees(a0,a1));
	ASSERT_EQUALM("a0==-15",-15,a0);
	ASSERT_EQUALM("a1==-27",-27,a1);

	SpatialVector u = SpatialVector(2.0,0.,0.);
	float64 l0 = -1, l1 = -1;
	try {
		u.getLatLonDegrees(l0,l1);
	} catch(SpatialException e) {
		ASSERT_EQUALM("LatLon from non-unit vector.",
				"SpatialVector::getLatLonDegrees::ERROR Calculating lat-lon-degrees from a non-unit vector.",
				e.what());
	}
}

/**
 *  Test htm range iterator.
 *
 * Note iter.nextSymbolic(buffer) increments its way through the intervals.
 *
 */
void testRangeIterator() {

	HtmRange *htmR = new HtmRange;
	Key keys[6] = {12682,12683,12690,12691,12697,12698};
	for( int i=0; i < 6; i+=2 ) {
		htmR->addRange(keys[i],keys[i+1]);
	}
	//    htmR->setEncoding(new BitShiftNameEncoding());
	htmR->setSymbolic(true);
	HtmRangeIterator iter(htmR);
	char buffer[80];
	int k=0;
	char expected[80], found[80];
	while (iter.hasNext()) {
		strcpy(expected,htmR->encoding->nameById(keys[k++]));
		strcpy(found,iter.nextSymbolic(buffer));
		//		cout << "k=" << k-1
		//				<< " ( " << expected << " vs. " << found << " )"
		//				<< " keys[k]= " << keys[k-1] << endl << flush;
		ASSERT_EQUALM("Checking range iterator for iter="+to_string(k-1),expected,found);
		//		cout << iter.nextSymbolic(buffer) << endl << flush;
	}
}

//void intersectTwoRectangles(
//		const SpatialIndex  *index,
//		const SpatialVector *u0,
//		const SpatialVector *u1,
//		const SpatialVector *u2,
//		const SpatialVector *u3,
//		const SpatialVector *v0,
//		const SpatialVector *v1,
//		const SpatialVector *v2,
//		const SpatialVector *v3
//		) {
//	int htmIdLevel = index->getLeafLevel();
//
//	SpatialDomain domain1 = SpatialDomain(index);
//	domain1.setOlevel(htmIdLevel); // Note this sets the olevel on the convexes.
//
//	SpatialDomain domain2 = SpatialDomain(index);
//	domain2.setOlevel(htmIdLevel); // Note this sets the olevel on the convexes.
//
//	cout << "1" << flush;
//	if(true){
////		SpatialVector *v0 = VectorFromLatLonDegrees(30.0,20.0);
////		SpatialVector *v1 = VectorFromLatLonDegrees(30.0,-10.0);
////		SpatialVector *v2 = VectorFromLatLonDegrees(20.0,0.0);
////		SpatialVector *v3 = VectorFromLatLonDegrees(20.0,30.0);
//		float r = 0.0;
//		float g = 1.0;
//		float b = 1.0;
////		viz->addRectangle(*u0,*u1,*u2,*u3,r,g,b);
//
//		RangeConvex rc = RangeConvex(u0,u1,u2,u3);
//		//	cout << "nConstraints: " << rc->numConstraints() << endl << flush;
//		//	SpatialConstraint *sc = new SpatialConstraint(SpatialVector(0.,0.,1.),0.5);
//		//	viz->addConstraint(*sc,1.0,1.0,1.0);
//		//	rc->add(*sc);
//		rc.setOlevel(htmIdLevel); // Note this is supposed to be done when added to the domain.
//		domain1.add(rc);
//
//	}
//	cout << "2" << flush;
//	if(true){
////		SpatialVector *v0 = VectorFromLatLonDegrees(10.0,0.0);
////		SpatialVector *v1 = VectorFromLatLonDegrees(30.0,-10.0);
////		SpatialVector *v2 = VectorFromLatLonDegrees(60.0,10.0);
////		SpatialVector *v3 = VectorFromLatLonDegrees(40.0,20.0);
//		float r = 1.0;
//		float g = 1.0;
//		float b = 0.0;
////		viz->addRectangle(*v0,*v1,*v2,*v3,r,g,b);
//
//		RangeConvex rc = RangeConvex(v0,v1,v2,v3);
//		//	cout << "nConstraints: " << rc->numConstraints() << endl << flush;
//		//	SpatialConstraint *sc = new SpatialConstraint(SpatialVector(0.,0.,1.),0.5);
//		//	viz->addConstraint(*sc,1.0,1.0,1.0);
//		//	rc->add(*sc);
//		rc.setOlevel(htmIdLevel); // Note this is supposed to be done when added to the domain.
//		domain2.add(rc);
//	}
//
////		return;
//	cout << "3" << flush;
//	bool varlen_individualHTMIds = false; // true for individuals, false for ranges
//
//	cout << "." << flush;
////	HtmRange range1 = HtmRange();
//	HtmRange range1;
//	range1.purge();
//	bool overlap1 = domain1.intersect(index,&range1,varlen_individualHTMIds);
//	cout << "." << flush;
//	range1.defrag();
//	range1.reset();
//	if(range1.nranges()==0)return;
//
//	cout << "." << flush;
//	HtmRange range2 = HtmRange();
//	range2.purge();
//	bool overlap2 = domain2.intersect(index,&range2,varlen_individualHTMIds);
//	cout << "." << flush;
//	range2.defrag();
//	range2.reset();
//	if(range2.nranges()==0)return;
//
//	Key lo = 0, hi = 0;
//	uint64 indexp = 0;
//
//	cout << "4" << flush;
//	cout << " overlaps: " << overlap1 << " " << overlap2 << ";" << flush;
//	cout << endl << flush;
//
//	cout << " range1.ranges(): " << range1.nranges() << endl << flush;
//	range1.reset();
//	indexp = range1.getNext(lo,hi);
//	cout << " range1.lo,hi " << lo << " " << hi << endl << flush;
//	cout << "        level " << levelOfId(lo) << endl << flush;
//
//	cout << " range2.ranges(): " << range2.nranges() << endl << flush;
//	range2.reset();
//	indexp = range2.getNext(lo,hi);
//	cout << " range2.lo,hi " << lo << " " << hi << endl << flush;
//	cout << "        level " << levelOfId(lo) << endl << flush;
//
//	HtmRange *resultRange = HTMRangeAtLevelFromIntersection(htmIdLevel,&range1,&range2);
//
//	cout << "5" << flush;
//	HtmRange *range = resultRange;
//	range->reset();
//	cout << "6" << flush;
//	lo=0; hi=0;
//	indexp = range->getNext(lo,hi);
//	SpatialVector x1,x2,x3;
//
//	cout << "7" << flush;
////		if(indexp) //?
//	cout << "lo,hi: " << lo << " " << hi << endl << flush;
//	cout << "indexp: " << indexp << endl << flush;
//
//	delete resultRange;
//}
//
//void testIntersectionRectangles() {
//
//}

/**
 * Cover the level and depth calculations underlying SpatialIndex's nodes_.
 */
void testIndexLevel() {
	BitShiftNameEncoding *name = new BitShiftNameEncoding();
	//	cout << "S0 =0x" << hex << name->idByName("S0") << endl << flush << dec;
	//	cout << "S00=0x" << hex << name->idByName("S00") << endl << flush << dec;
	int maxLevel   = 4;
	int saveLevel  = 6;
	for(maxLevel=1;maxLevel<9;maxLevel++) {
		SpatialIndex *index = new SpatialIndex(maxLevel,saveLevel);
		cout
		<< " maxLevel=" << maxLevel
		<< " saveLevel=" << saveLevel
		<< " index->layers_.size=" << index->layersSize()
		<< " index->maxLevel=" << index->getMaxlevel()
		<< " index->buildLevel=" << index->getBuildLevel()
		<< " symbolic-name-base=0x"
		<< hex << index->idAtNodeIndex(9)
		<< dec << " " << index->nameById(index->idAtNodeIndex(9))
		//		<< " depth=" << depthOfName(index->nameById(index->idAtNodeIndex(9)))
		<< " level=" << name->levelById(index->idAtNodeIndex(9))
		<< endl << flush;
	}
	delete name;
}

void testEmbeddedLevelNameEncoding() {
	EmbeddedLevelNameEncoding *name = new EmbeddedLevelNameEncoding();

	ASSERT_EQUALM("Encoding type","EmbeddedLevelNameEncoding",name->getEncodingType());

	//	ASSERT_EQUALM("id to name","nothing",name->nameById(0));
	uint64 id = name->idByName("N3");
	//	cout <<  "id: N3=0x" << hex << id << dec << " " << id << endl << flush;
	// 17293822569102704640ull
	ASSERT_EQUALM("name to id",0xf000000000000000ull,id);

	id = name->idByName("S3");
	//	cout <<  "id: S3=0x" << hex << id << dec << " " << id << endl << flush;
	// 12682136550675316736ull
	ASSERT_EQUALM("name to id",0xb000000000000000ull,id);

	id = name->idByName("S33");
//	cout <<  "id: S33=0x" << hex << id << dec << " " << id << endl << flush;
	// 13546827679130451969ull
	ASSERT_EQUALM("name to id",0xbc00000000000001ull,id);

	id = name->idByName("S303");
//	cout <<  "id: S303=0x" << hex << id << dec << " " << id << endl << flush;
	// 12898309332789100546ull
	ASSERT_EQUALM("name to id",0xb300000000000002ull,id);

	id = name->idByName("S00000");
	//	cout <<  "id: S00000=0x" << hex << id << dec << " " << id << endl << flush;
	//	id: S00000=0x8000000000000004 9223372036854775812
	ASSERT_EQUALM("name to id",0x8000000000000004ull,id);

	ASSERT_EQUALM("level of id",4,name->levelById(name->idByName("S00000")));

	const char *nameString = "N20001123123123123";
	id = name->idByName(nameString);
	//	cout <<  "id: " << nameString << "=0x" << hex << id << ", " << dec << " " << id << endl << flush;

	name->setName(nameString);
	//    cout << "name: " << nameString << endl << flush
	//    		<< hex << " 0x" << setw(20) << name->getId() << endl << flush
	//			       << " 0x" << setw(20) << name->bareId() << endl << flush;

	BitShiftNameEncoding *bitShiftName = new BitShiftNameEncoding(nameString);
	ASSERT_EQUALM("Encoding type","BitShiftedNameEncoding",bitShiftName->getEncodingType());

	NameEncoding *testName = bitShiftName;
	ASSERT_EQUALM("Encoding type","BitShiftedNameEncoding",testName->getEncodingType());

	bitShiftName->setName(nameString);
	//    cout << "bitShiftName: " << nameString << endl << flush
	//    		<< hex << " 0x" << setw(20) << bitShiftName->getId() << endl << flush
	//			       << " 0x" << setw(20) << bitShiftName->bareId() << endl << flush;

	ASSERT_EQUALM("compare bare id's to old-style",bitShiftName->bareId(),name->bareId());

	{
		string failureMessage = "'";
		string foundName ="'";
		try {
			foundName += name->nameById(0);
		} catch (SpatialFailure failure) {
			failureMessage += failure.what();
		}
		foundName += "'";
		failureMessage += "'";
		ASSERT_EQUALM("INVALID ID 0","'EmbeddedLevelNameEncoding::nameById-INVALID_ID_0'",failureMessage);

		foundName ="'";
		foundName += name->nameById(name->idByName("N3"));
		foundName +="'";
		ASSERT_EQUALM("Test N3","'N3'",foundName);

		foundName = "'";
		foundName += name->nameById(name->idByName("S3123123"));
		foundName +="'";
		ASSERT_EQUALM("Test S3123123","'S3123123'",foundName);

		ASSERT_EQUALM("S02 > N01",false,name->idByName("S02") > name->idByName("N01"));
		ASSERT_EQUALM("N01 > N01",false,name->idByName("N01") > name->idByName("N01"));
		ASSERT_EQUALM("S02 < N01",true,name->idByName("S02") < name->idByName("N01"));

		ASSERT_EQUALM("N0123 == N0123",true,name->idByName("N0123") == name->idByName("N0123"));
		ASSERT_EQUALM("N0123 != N0123",false,name->idByName("N0123") != name->idByName("N0123"));
		ASSERT_EQUALM("N01230 != N0123",true,name->idByName("N01230") != name->idByName("N0123"));

		EmbeddedLevelNameEncoding *n0  = new EmbeddedLevelNameEncoding("N01230");
		EmbeddedLevelNameEncoding *n1  = new EmbeddedLevelNameEncoding("N0123");
//		ASSERT_EQUALM("N01230.level < N0123.level?",false,n0->levelLessThan(*n1));
//		ASSERT_EQUALM("N0123.level < N01230.level?",true,n1->levelLessThan(*n0));
		ASSERT_EQUALM("N01230.level == 4",4,n0->getLevel());
		ASSERT_EQUALM("N0123.level == 3",3,n1->getLevel());
		ASSERT_EQUALM("greatest common level N01230 and N0123",3,n0->greatestCommonLevel(*n1));
		n1->setName("N02230");
		ASSERT_EQUALM("greatest common level N01230 and N02230",0,n0->greatestCommonLevel(*n1));
		n1->setName("S01230");
		ASSERT_EQUALM("greatest common level N01230 and S01230",-1,n0->greatestCommonLevel(*n1));
		n1->setName("N0123");
//		cout << "'" << n0->greatestCommonName(*n1) << "'" << endl << flush;
//		cout << "'" << "N0123" << "'" << endl << flush;
		string expected = "N0123", found=n0->greatestCommonName(*n1);
		ASSERT_EQUALM("greatest common name N01230 and N0123",expected,found);
		n1->setName("N02230");
		expected = "N0"; found=n0->greatestCommonName(*n1);
		ASSERT_EQUALM("greatest common name N01230 and N02230",expected,found);
		n1->setName("N01220");
		expected = "N012"; found=n0->greatestCommonName(*n1);
		ASSERT_EQUALM("greatest common name N01230 and N01220",expected,found);
	}
}

void testRangeManipulation() {

	{
		HtmRange h;
		h.addRange(10,15);
		//	cout << "10..15--" << endl << flush;
		//	h.print(HtmRange::BOTH,cout,false);
		h.addRange(12,20);
		h.reset();
		KeyPair kp = h.getNext();
		ASSERT_EQUALM("kp.set == 1", 1,kp.set);
		ASSERT_EQUALM("kp.lo == 10",10,kp.lo);
		// ASSERT_EQUALM("kp.hi == 15",15,kp.hi); // We now have merge semantics instead of add.
		ASSERT_EQUALM("kp.hi == 20",20,kp.hi); // We now have merge semantics instead of add.
		kp = h.getNext();
		// ASSERT_EQUALM("kp.set == 0", 1,kp.set); // We now have merge semantics instead of add.
		ASSERT_EQUALM("kp.set == 0", 0,kp.set); // We now have merge semantics instead of add.
		// ASSERT_EQUALM("kp.lo == 12",12,kp.lo); // We now have merge semantics instead of add.
		// ASSERT_EQUALM("kp.hi == 20",20,kp.hi); // We now have merge semantics instead of add.
		kp = h.getNext();
		ASSERT_EQUALM("kp.set == 0", 0,kp.set);
		h.reset();
		h.defrag();
//		h.print(HtmRange::BOTH,cout,false);
	}

	{
		HtmRange h;
		h.addRange(10,15);
		//	cout << "10..15--" << endl << flush;
		//	h.print(HtmRange::BOTH,cout,false);
		h.mergeRange(12,20);
		h.reset();
		KeyPair kp = h.getNext();
		ASSERT_EQUALM("kp.lo == 10",10,kp.lo);
		ASSERT_EQUALM("kp.hi == 20",20,kp.hi);
		ASSERT_EQUALM("kp.set == 1", 1,kp.set);
		kp = h.getNext();
		ASSERT_EQUALM("kp.set == 0", 0,kp.set);
	}




//	cout << "kp: " << kp.lo << " " << kp.hi << " " << kp.set << endl << flush;
//	cout << "kp: " << kp.lo << " " << kp.hi << " " << kp.set << endl << flush;

//	h.addRange(12,20);
//	cout << "10..20-1" << endl << flush;
//	h.print(HtmRange::BOTH,cout,false);
//	h.defrag();
//	cout << "10..20-2" << endl << flush;
//	h.print(HtmRange::BOTH,cout,false);
//	HtmRange i;
//	i.addRange(15,25);
//	h.addRange(&i);
//	cout << "10..25-1" << endl << flush;
//	h.print(HtmRange::BOTH,cout,false);
}

void testRangeDuplication() {
	HtmRange h;
//	long losCount, hisCount;

	h.addRange(100,110);
//	losCount = h.my_los->getCount(); hisCount = h.my_his->getCount();
	ASSERT_EQUAL(1,h.my_los->getCount());
	ASSERT_EQUAL(1,h.my_his->getCount());

	h.addRange(105,110);
//	losCount = h.my_los->getCount(); hisCount = h.my_his->getCount();
	ASSERT_EQUAL(1,h.my_los->getCount());
	ASSERT_EQUAL(1,h.my_his->getCount());


	h.addRange(100,103);
	ASSERT_EQUAL(1,h.my_los->getCount());
	ASSERT_EQUAL(1,h.my_his->getCount());

	h.addRange(200,300);
	ASSERT_EQUAL(2,h.my_los->getCount());
	ASSERT_EQUAL(2,h.my_his->getCount());

//	h.defrag();
//	ASSERT_EQUAL(2,h.my_los->getCount());
//	ASSERT_EQUAL(1,h.my_his->getCount());
//	cout << "101" << endl << flush;

	Key lo, hi;
	h.reset();

	int iRange = 0;
	int iTestCount = 0;
	int indexp = h.getNext(lo,hi);
	if(indexp) {
		do {
			switch(iRange) {
			case 0 :
				ASSERT_EQUAL(100,lo);
				ASSERT_EQUAL(110,hi);
				iTestCount++;
				break;
			case 1 :
				ASSERT_EQUAL(200,lo);
				ASSERT_EQUAL(300,hi);
				iTestCount++;
				break;
			default :
				break;
			}
			iRange++;
		} while (h.getNext(lo,hi));
	}

	ASSERT_EQUAL(2,h.nranges());
	ASSERT_EQUAL(2,iTestCount);
	ASSERT_EQUAL(2,iRange);
}

/**
 * Add rotation about an axis to SpatialVector.
 */
void testRotation() {
	//	ASSERT_EQUALM("Failing test",0,1);
	stringstream msgStream;
	float64 tolerance = 1.0e-15;
//	SpatialVector origin(0.0,0.0,0.0);
	const SpatialVector zHat(0.0,0.0,1.0), xHat(1.0,0.0,0.0), yHat(0.0,1.0,0.0);

	float64 theta;
	SpatialVector start, axis, rot, expected;

	theta = gPi;
	start = xHat;
	axis  = zHat;
	expected = -1.0*xHat;
	rot = start.rotatedAbout(axis,theta);
	tolerance = 1.0e-15;
	msgStream << "Reverse xHat: <";
	msgStream << "start: " << start << ", ";
	msgStream << "axis: " << axis << ", ";
	msgStream << "th: " << theta << ", ";
	msgStream << "r: " << rot << ", ";
	msgStream << "tol: " << tolerance;
	msgStream << ">";
	ASSERT_EQUALDM(msgStream.str().c_str(),expected,rot,tolerance);
	msgStream.str(string()); msgStream.clear();

	theta = gPi*0.5;
	start = xHat;
	axis  = zHat;
	expected = yHat;
	rot = start.rotatedAbout(axis,theta);
	tolerance = 1.0e-15;
	msgStream << "Rotate xHat to yHat: <";
	msgStream << "start: " << start << ", ";
	msgStream << "axis: " << axis << ", ";
	msgStream << "th: " << theta << ", ";
	msgStream << "r: " << rot << ", ";
	msgStream << "tol: " << tolerance;
	msgStream << ">";
	ASSERT_EQUALDM(msgStream.str().c_str(),expected,rot,tolerance);
	msgStream.str(string()); msgStream.clear();

	theta = -gPi*0.5;
	start = zHat;
	axis  = xHat;
	expected = yHat;
	rot = start.rotatedAbout(axis,theta);
	tolerance = 1.0e-15;
	msgStream << "Rotate zHat to yHat: <";
	msgStream << "start: " << start << ", ";
	msgStream << "axis: " << axis << ", ";
	msgStream << "th: " << theta << ", ";
	msgStream << "r: " << rot << ", ";
	msgStream << "tol: " << tolerance;
	msgStream << ">";
	ASSERT_EQUALDM(msgStream.str().c_str(),expected,rot,tolerance);
	msgStream.str(string()); msgStream.clear();

}


void testNeighbors() {

	int level         = 5;
	int saveLevel     = 5;
	htmInterface *htm_ = new htmInterface(level,saveLevel);
	const SpatialIndex index = htm_->index();
	const char *htmName = "N012023";
	uint64 htmID = index.idByName(htmName);
	uint64 htmIDExpected = 12683;
	ASSERT_EQUALM("N012023 == 12683?",htmIDExpected,htmID);
	ASSERT_EQUALM("level(N012023) == 5",5,levelOfName(htmName));
	ASSERT_EQUALM("level(12683)   == 5",5,levelOfId(htmID));

	uint64 neighbors[3];
	uint64 htmId;
	htmId = htmID;

	htmId = 12683;
	index.NeighborsAcrossEdgesFromHtmId( neighbors, htmId);

//	cout
//	<< "Neighbors of " << (htmId) << " are "
//	<< (neighbors[0]) << ", "
//	<< (neighbors[1]) << ", "
//	<< (neighbors[2]) << endl << flush;
//
//	cout
//	<< "Neighbors of " << index.nameById(htmId) << " are "
//	<< index.nameById(neighbors[0]) << ", "
//	<< index.nameById(neighbors[1]) << ", "
//	<< index.nameById(neighbors[2]) << endl << flush;

//	htmId = 12681;
//	index.NeighborsAcrossEdgesFromHtmId( neighbors, htmId);
//
//	cout
//	<< "Neighbors of " << (htmId) << " are "
//	<< (neighbors[0]) << ", "
//	<< (neighbors[1]) << ", "
//	<< (neighbors[2]) << endl << flush;
//
//	cout
//	<< "Neighbors of " << index.nameById(htmId) << " are "
//	<< index.nameById(neighbors[0]) << ", "
//	<< index.nameById(neighbors[1]) << ", "
//	<< index.nameById(neighbors[2]) << endl << flush;
//
//	htmId = 13190;
//	index.NeighborsAcrossEdgesFromHtmId( neighbors, htmId);
//
//	cout
//	<< "Neighbors of " << (htmId) << " are "
//	<< (neighbors[0]) << ", "
//	<< (neighbors[1]) << ", "
//	<< (neighbors[2]) << endl << flush;
//
//	cout
//	<< "Neighbors of " << index.nameById(htmId) << " are "
//	<< index.nameById(neighbors[0]) << ", "
//	<< index.nameById(neighbors[1]) << ", "
//	<< index.nameById(neighbors[2]) << endl << flush;
//
//	htmId = 12680;
//	index.NeighborsAcrossEdgesFromHtmId( neighbors, htmId);
//
//	cout
//	<< "Neighbors of " << (htmId) << " are "
//	<< (neighbors[0]) << ", "
//	<< (neighbors[1]) << ", "
//	<< (neighbors[2]) << endl << flush;
//
//	htmId = 12682;
//	index.NeighborsAcrossEdgesFromHtmId( neighbors, htmId);
//
//	cout
//	<< "Neighbors of " << (htmId) << " are "
//	<< (neighbors[0]) << ", "
//	<< (neighbors[1]) << ", "
//	<< (neighbors[2]) << endl << flush;

	htmId = 12683;
	uint64 neighborsV[9];
	index.NeighborsAcrossVerticesFromHtmId( neighborsV, htmId);

//	cout << "Neighbors Across Vertices of " << (htmId) << " are ";
//	for(int i=0; i<9; i++) {
//		cout << (neighborsV[i]) << " ";
//	}
//	cout << endl << flush;
//
//	cout << "Neighbors Across Vertices of " << index.nameById(htmId) << " are ";
//	for(int i=0; i<9; i++) {
//		cout << index.nameById(neighborsV[i]) << " ";
//	}
//	cout << endl << flush;

	ASSERT_EQUALM("Neighbor face 23",12680,neighbors[0]);
	ASSERT_EQUALM("Neighbor face 13",12681,neighbors[1]);
	ASSERT_EQUALM("Neighbor face 12",12682,neighbors[2]);

	ASSERT_EQUALM("Neighbor across vertex 1,13",12685,neighborsV[0]);
	ASSERT_EQUALM("Neighbor across vertex 1",   12687,neighborsV[1]);
	ASSERT_EQUALM("Neighbor across vertex 1,12",12684,neighborsV[2]);

	ASSERT_EQUALM("Neighbor across vertex 2,12",12725,neighborsV[3]);
	ASSERT_EQUALM("Neighbor across vertex 2",   12727,neighborsV[4]);
	ASSERT_EQUALM("Neighbor across vertex 2,23",12724,neighborsV[5]);

	ASSERT_EQUALM("Neighbor across vertex 3,23",13188,neighborsV[6]);
	ASSERT_EQUALM("Neighbor across vertex 3",   13191,neighborsV[7]);
	ASSERT_EQUALM("Neighbor across vertex 3,13",13190,neighborsV[8]);
}

void hstmIndexLibrarySketch() {
	HstmIndex hIndex = HstmIndex();
	// cout << "10 " << flush;
	// cout << hIndex.range->nranges() << endl << flush;
	ASSERT_NOT_EQUAL_TO((HtmRange*)0,hIndex.range);
	ASSERT_EQUAL(0,hIndex.range->nranges());
	hIndex.range->addRange(1,3);
	ASSERT_EQUAL(1,hIndex.range->nranges());
	stringstream ss;
	ss << "'" << *(hIndex.range) << "'";
	// cout << "20: "<< ss.str() << endl << flush;
	ASSERT_EQUAL("'1 3'",ss.str());
	// ASSERT_EQUALM("False test",false,true);
}

void runSuite(int argc, char const *argv[]){
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<>  > lis(xmlfile.out);
	cute::suite s;
	s.push_back(CUTE(precisionTest));
	s.push_back(CUTE(lookupID));
	s.push_back(CUTE(pointById));
	s.push_back(CUTE(idByPoint1));
	s.push_back(CUTE(lookupOnTwoSaveLevels));
	s.push_back(CUTE(lookupOnMultipleLevels));
	s.push_back(CUTE(idReallyDeep));
	s.push_back(CUTE(idByName));
	s.push_back(CUTE(checkBitShiftNameEncoding0));
	s.push_back(CUTE(testRangeContains));
	s.push_back(CUTE(testRangeIterator));
	s.push_back(CUTE(testEmbeddedLevelNameEncoding));
	s.push_back(CUTE(testRotation));
	s.push_back(CUTE(testRangeManipulation));
	s.push_back(CUTE(testRangeDuplication));
	s.push_back(CUTE(testLatLonDegrees));
	s.push_back(CUTE(testNeighbors));
	s.push_back(CUTE(hstmIndexLibrarySketch));


	if(false) { // Lots of diagnostic output in the following.
		s.push_back(CUTE(testRange));
		s.push_back(CUTE(testIndexLevel));
	}

	cute::makeRunner(lis,argc,argv)(s, "testTestSuite");
}

int main(int argc, char const *argv[]){
	runSuite(argc,argv);
}

