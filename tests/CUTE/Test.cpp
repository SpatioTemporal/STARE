
#include <string>
#include <iostream>
#include <iomanip>

#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"

// TODO #include the headers for the code you want to test
//#include "SpatialGeneral.h"
#include "SpatialException.h"
#include "SpatialIndex.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"

#include "HtmRangeIterator.h"
#include "BitShiftNameEncoding.h"

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
	int depth          = 4;
	int saveDepth      = 5;
	htmInterface *htm_ = new htmInterface(depth,saveDepth);
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

void lookupOnTwoSaveDepths() {

	float64 x =  0.50350931157595497;
	float64 y = -0.6109330459045055;
	float64 z = -0.6109330459045055;

	int depth          = 4;
	int saveDepth      = 5;
	htmInterface *htm0_ = new htmInterface(depth,saveDepth);
	int id0 = htm0_ ->lookupID(x,y,z);

	depth          = 4;
	saveDepth      = 2;
	htmInterface *htm1_ = new htmInterface(depth,saveDepth);
	int id1 = htm1_ ->lookupID(x,y,z);

    ASSERT_EQUALM("id from 2 saveDepths: ",id0,id1);

}

//int depthOfName(const char name[]) { return strlen(name)-1; }
#define ASSERT_EQUALM_NAMEBYID_(msg,expected,index,id,name){char *n; n=index.nameById(id,name); ASSERT_EQUALM(msg,expected,name);}
#define INDEX_(name){htm[depthOfName(name)]->index();}
#define PRINT_ID(msg,htm,name) {	SpatialIndex index = htm[depthOfName(name)]->index(); cout << msg << " indexDepth: " << index.getMaxlevel() << " id: " << flush; cout << index.idByName(name) << " name: " << name << " nameDepth: " << depthOfName(name); SpatialVector v; index.pointById(v,index.idByName(name)); cout << " v: " << v << endl << flush;}

void lookupOnMultipleDepths() {

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

	int depth          = 3;
	int saveDepth      = 2;

	index0_ = htm[depth]->index();
	id0 =     htm[depth]->lookupID(x,y,z);

	ASSERT_EQUALM("S3333: id0==767: ",767,id0);

	depth          = 4;
	saveDepth      = 5;
	index1_ = htm[depth]->index();
	id1 =     htm[depth]->lookupID(x,y,z);

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
	depth = 1;
	depth = depthOfName("S00");
//	cout << hex;
	SpatialIndex index = htm[depth-1]->index();
	for(int i = 1; i < 38; i++) {
		cout << "i: " << i << " " ;
		cout << "d: " << depth << " " ;
		int idx = index.indexAtNodeIndex(i);
		cout << "idx; " << idx << " ";
		int id =  index.idAtNodeIndex(i); // nodes_[i].id_;
		cout << hex << "id: x" << id << " " << dec;
		char *n = new char[1024];
		index.nameById(id,n);
		cout << "n: " << n << " ";
		cout << hex << "ibn: x" << index.idByName(n) << " " << dec;
		cout << "d(n): " << depthOfName(n) << " ";
		cout << "leafN: " << index.leafNumberById(id) << " " ;
		cout << endl << flush;
	}

	cout << "index.layers_.size(): " << index.layersSize() << endl << flush;
	for(int i=0; i < index.layersSize(); i++){
//	for(int i=0; i < depth; i++){
		cout << "i: " << i << " ";
		cout << "d: " << depth << " " ;
		cout << "fIndex: " << index.firstIndexOfLayerAtDepth(depth) << " ";
		cout << endl << flush;
	}
*/
	char cTmp[1024];
	ASSERT_EQUALM("id0 to name to index1's id1",id0,index1_.idByName(index0_.nameById(id0,cTmp)));
	ASSERT_EQUALM("id1 to name to index0's id0",id1,index0_.idByName(index1_.nameById(id1,cTmp)));
}

void idReallyDeep() {

	uint32 level = 27;
	uint32 shift = level*2+1;
	uint64 htmID0 = 1;
	htmID0 = htmID0 << shift;
	char expected[1024];
	for(int i=0;i<1024;i++)expected[i] = 0;
	expected[0] = 'S';
	uint64 htmID=2;
	for(int i=1; i<level+1; i++) {
		uint64 triangle = rand() % 4;
//		triangle = 0;
		expected[i] = '0' + triangle;
		htmID = htmID << 2;
		htmID += triangle;
//		cout << " " << i << " " << triangle << " " << expected[i] << " " << expected << " " << hex << htmID << dec << endl << flush;
	}
	expected[level+1]=0;
/*
	cout << " htmID0 " << htmID0 << " " << hex << htmID0 << dec << endl << flush;
	cout << " htmID  " << htmID << " " << hex << htmID << dec << endl << flush;
	cout << " expec  " << expected << endl << flush;
*/
	htmInterface *htm = new htmInterface(level,5);
	SpatialIndex index = htm->index();
	char foundName[1024]; for(int i=0;i<1024;i++)foundName[i] = 0;
	try {
		index.nameById(htmID,foundName);
	} catch (const SpatialException & e) {
		cout << "Exception " << e.what() << " n: " << foundName << endl << flush;
	}
/*
	cout << "level " << level << endl << flush;
	cout << "htmID " << htmID << endl << flush;
	cout << "fName  " << foundName  << endl << flush;
*/
	ASSERT_EQUALM("depth:    ",level,depthOfName(foundName));
	ASSERT_EQUALM("fName:    ",expected,foundName);
	ASSERT_EQUALM("htmID^-1: ",index.idByName(foundName),htmID);

	index.setMaxlevel(depthOfId(htmID)-1);
/*
	cout << " depthHtmId: " << depthOfId(htmID) << endl << flush;
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
	int depth          = 4;
	int saveDepth      = 5;
	htmInterface *htm_ = new htmInterface(depth,saveDepth);
	float64 x = 0.50350942389316267;
	float64 y = -0.61093299962057024;
	float64 z = -0.61093299962057024;
	double tolerance = 1.0e-14;
	int id_ = htm_->lookupID(x,y,z);
	SpatialVector v_;
//	htm_->pointById(v_,id_);
	htm_->pointById_mlr1(v_,id_);
//	ASSERT_EQUALM("SpatialVectors x: ",0,2.0*(v_.x()-x)/abs(v_.x()+x));
//	componentCheck(z);

	cout << hex;
	ASSERT_LESSM("SpatialVectors x: ",abs(x-v_.x()),tolerance);
	ASSERT_EQUALDM("SpatialVectors: ",SpatialVector(x,y,z),v_,tolerance);
	cout << dec;
}

/**
 * Try an example found in the source's comments.
 */
void idByName() {
	int depth         = 4;
	int saveDepth     = 5;
	htmInterface *htm_ = new htmInterface(depth,saveDepth);
	const SpatialIndex index = htm_->index();
	const char *htmName = "N012023";
	uint64 htmID = index.idByName(htmName);
	uint64 htmIDExpected = 12683;
	ASSERT_EQUALM("N012023 == 12683?",htmIDExpected,htmID);
	ASSERT_EQUALM("depth(N012023) == 5 or 6th?",6,depthOfName(htmName));
	ASSERT_EQUALM("depth(12683)   == 5 or 6th?",6,depthOfId(htmID));

	htm_->changeDepth(depthOfId(htmID)-1,saveDepth);
	const SpatialIndex index1 = htm_->index(); // Update the index.
	int htmIndexLayerSize = index1.layersSize();
	ASSERT_EQUALM("htm_ index number of levels==depth==6",depthOfId(htmID),htmIndexLayerSize);

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
		failureMessage += "'";
	}
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


/**
 * Cover the level and depth calculations underlying SpatialIndex's nodes_.
 */
void testIndexLevelAndDepth() {
	BitShiftNameEncoding *name = new BitShiftNameEncoding();
	cout << "S0 =0x" << hex << name->idByName("S0") << endl << flush << dec;
	cout << "S00=0x" << hex << name->idByName("S00") << endl << flush << dec;
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
		<< " depth=" << depthOfName(index->nameById(index->idAtNodeIndex(9)))
		<< " level=" << name->levelById(index->idAtNodeIndex(9))
		<< endl << flush;
	}
	delete name;
}

void runSuite(int argc, char const *argv[]){
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<>  > lis(xmlfile.out);
	cute::suite s;
	s.push_back(CUTE(precisionTest));
	s.push_back(CUTE(lookupID));
	s.push_back(CUTE(pointById));
	s.push_back(CUTE(lookupOnTwoSaveDepths));
	s.push_back(CUTE(lookupOnMultipleDepths));
	s.push_back(CUTE(idReallyDeep));
	s.push_back(CUTE(idByName));
	s.push_back(CUTE(checkBitShiftNameEncoding0));
//	s.push_back(CUTE(testRange));
	s.push_back(CUTE(testRangeIterator));
	s.push_back(CUTE(testIndexLevelAndDepth));
	cute::makeRunner(lis,argc,argv)(s, "testTestSuite");
}

int main(int argc, char const *argv[]){
    runSuite(argc,argv);
}

