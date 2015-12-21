
#include <iostream>
#include <iomanip>

#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"

// TODO #include the headers for the code you want to test
#include "SpatialException.h"
#include "SpatialIndex.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"

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

int depthOfName(const char name[]) { return strlen(name)-1; }
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
	cute::makeRunner(lis,argc,argv)(s, "testTestSuite");
}

int main(int argc, char const *argv[]){
    runSuite(argc,argv);
}

