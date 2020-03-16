//#     Filename:       SpatialIndex.cpp
//#
//#     The SpatialIndex class is defined here.
//#
//#     Author:         Peter Z. Kunszt based on A. Szalay's code
//#     
//#     Date:           October 15, 1998
//#
//#		Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#
//#     Modification History:
//#
//#     Oct 18, 2001 : Dennis C. Dinge -- Replaced ValVec with std::vector
//#		Jul 25, 2002 : Gyorgy Fekete -- Added pointById()
//#

#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include "SpatialIndex.h"

#ifdef _WIN32
#include <malloc.h>
#else
#ifdef __macosx
#include <sys/malloc.h>
#else
#include <stdlib.h>
#endif
#endif

#include <stdio.h>
#include <cmath>
// ===========================================================================
//
// Macro definitions for readability
//
// ===========================================================================

#define N(x) nodes_[(x)]
#define V(x) vertices_[nodes_[index].v_[(x)]]
#define IV(x) nodes_[index].v_[(x)]
#define W(x) vertices_[nodes_[index].w_[(x)]]
#define IW(x) nodes_[index].w_[(x)]
#define ICHILD(x) nodes_[index].childID_[(x)]

#define IV_(x) nodes_[index_].v_[(x)]
#define IW_(x) nodes_[index_].w_[(x)]
#define ICHILD_(x) nodes_[index_].childID_[(x)]
#define IOFFSET 9

using namespace std;

// ===========================================================================
//
// Member functions for class SpatialIndex
//
// ===========================================================================
//////////////////ISINSIDE/////////////////////////////////////////////////
// Test whether a vector is inside a triangle. Input triangle has
// to be sorted in a counter-clockwise direction.
//
bool
SpatialIndex::isInside(const SpatialVector & v, const SpatialVector & v0,
		       const SpatialVector & v1, const SpatialVector & v2) const
{
  if( (v0 ^ v1) * v < -gEpsilon) return false;
  if( (v1 ^ v2) * v < -gEpsilon) return false;
  if( (v2 ^ v0) * v < -gEpsilon) return false;
  return true;
}
bool
isInside(const SpatialVector & v, const SpatialVector & v0,
	 const SpatialVector & v1, const SpatialVector & v2)
{
  if( (v0 ^ v1) * v < -gEpsilon) return false;
  if( (v1 ^ v2) * v < -gEpsilon) return false;
  if( (v2 ^ v0) * v < -gEpsilon) return false;
  return true;
}
float64 triple_product(const SpatialVector &a, const SpatialVector &b, const SpatialVector &c)
{
  return (a ^ b) * c;
}
float64 min_triangle_quality(
			     const SpatialVector & v, const SpatialVector & v0,
			     const SpatialVector & v1, const SpatialVector & v2)
{
  float64 d01 = triple_product(v0,v1,v);
  float64 d12 = triple_product(v1,v2,v);
  float64 d20 = triple_product(v2,v0,v);
  return min(d01,min(d12,d20));
}

/*
 * A discussion of determining if the projection of a point is within a triangle on a plane.
 * https://math.stackexchange.com/questions/544946/determine-if-projection-of-3d-point-onto-plane-is-within-a-triangle
 *
 * The Barycentric calculation used below is mentioned below.
 * https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
 * http://realtimecollisiondetection.net
 * Real-Time Collision Detection by Christer Ericson (Morgan Kaufmann, 2005).
 */

void print_vector(string name, SpatialVector v) {
  cout
    << setprecision(17)
    << setw(20)
    << scientific;

  cout << "SpatialVector " << name << " = SpatialVector( "
       << v.x() << ", "
       << v.y() << ", "
       << v.z() << " );" << endl << flush;
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
void Barycentric(
		 SpatialVector p, SpatialVector a, SpatialVector b, SpatialVector c,
		 float64 &u, float64 &v, float64 &w, bool verbose)
{
  SpatialVector v0 = b - a, v1 = c - a, v2 = p - a;
  float64 d00 = v0*v0;
  float64 d01 = v0*v1;
  float64 d11 = v1*v1;
  float64 d20 = v2*v0;
  float64 d21 = v2*v1;
  float64 idenom = 1.0/(d00 * d11 - d01 * d01);
  v = (d11 * d20 - d01 * d21) * idenom;
  w = (d00 * d21 - d01 * d20) * idenom;
  u = 1.0 - v - w;

  if(verbose) {
    cout
      << setprecision(17)
      << setw(20)
      << scientific
      << endl;
    cout << " - " <<  endl << flush;
    print_vector("a",a);
    print_vector("b",b);
    print_vector("c",c);
    print_vector("p",p);
    cout << " - " <<  endl << flush;
    cout << " - " <<  endl << flush;
    cout << "a " << a << endl << flush;
    cout << "b " << b << endl << flush;
    cout << "c " << c << endl << flush;
    cout << endl << flush;
    cout << "v0 = " << v0 << ", l = " << v0.length() << endl << flush;
    cout << "v1 = " << v1 << ", l = " << v1.length() << endl << flush;
    cout << "v2 = " << v2 << ", l = " << v2.length() << endl << flush;
    cout << endl << flush;
    cout << "d00    " << d00 << endl << flush;
    cout << "d01    " << d01 << endl << flush;
    cout << "d11    " << d11 << endl << flush;
    cout << "d20    " << d20 << endl << flush;
    cout << "d21    " << d21 << endl << flush;
    cout << "idenom " << idenom << endl << flush;
    cout << "v      " << v << endl << flush;
    cout << "w      " << w << endl << flush;
    cout << "u      " << u << endl << flush;
    cout << endl << flush;
  }
}
bool
SpatialIndex::isInsideBarycentric(
				  const SpatialVector & v, const SpatialVector & v0,
				  const SpatialVector & v1, const SpatialVector & v2,
				  bool verbose
				  ) const {
  float64 u,w,vv;
  Barycentric(v,v0,v1,v2,u,vv,w,verbose);
  return
    ((0 <= u) && (u <= 1)) &&
    ((0 <= w) && (w <= 1)) &&
    ((0 <= vv) && (vv <= 1)) ;
}
bool
isInsideBarycentric(
		    const SpatialVector & v, const SpatialVector & v0,
		    const SpatialVector & v1, const SpatialVector & v2,
		    bool verbose
		    ) {
  float64 u,w,vv;
  Barycentric(v,v0,v1,v2,u,vv,w,verbose);
  return
    ((0 <= u) && (u <= 1)) &&
    ((0 <= w) && (w <= 1)) &&
    ((0 <= vv) && (vv <= 1)) ;
}

void
checkVectors(const SpatialVector & v, const SpatialVector & v0,
	     const SpatialVector & v1, const SpatialVector & v2)
{

  float64
    a = (v0 ^ v1) * v,
    b = (v1 ^ v2) * v,
    c =  (v2 ^ v0) * v;

  bool d =
    (( a >= 0 )) &&
    (( b >= 0 )) &&
    (( c >= 0 ));

  cout
    << setprecision(17)
    << setw(20)
    << scientific
    << "checkVectors" << endl
    << " 01 " << a << endl
    << " 12 " << b << endl
    << " 20 " << c << endl
    << "res " << d << endl
    << flush;
}

/**
 * Construct the sub-triangle of index subTriangleIndex.
 *
 * @param v0
 * @param v1
 * @param v2
 * @param subTriangleIndex
 */
void partitionTriangle(
		       SpatialVector &v0,
		       SpatialVector &v1,
		       SpatialVector &v2,
		       uint64        subTriangleIndex) {

  // cout << "." << endl << flush;
  
  SpatialVector w0 = v1 + v2; w0.normalize();
  SpatialVector w1 = v0 + v2; w1.normalize();
  SpatialVector w2 = v1 + v0; w2.normalize();

  switch(subTriangleIndex) {
  case 0:
    v1 = w2;
    v2 = w1;
    break;
  case 1:
    v0 = v1;
    v1 = w0;
    v2 = w2;
    break;
  case 2:
    v0 = v2;
    v1 = w1;
    v2 = w0;
    break;
  case 3:
    v0 = w0;
    v1 = w1;
    v2 = w2;
    break;
  }
}

/**
 * Construct the subtriangle containing the point v and return the subtriangle
 * index so we know which partition contained the point.
 *
 * @param v
 * @param v0
 * @param v1
 * @param v2
 * @return
 */
int subTriangleIndexByPoint(
			    SpatialVector &v,
			    SpatialVector &v0,
			    SpatialVector &v1,
			    SpatialVector &v2
			    ) {
  int subTriangleIndex = -1;

  SpatialVector w0 = v1 + v2; w0.normalize();
  SpatialVector w1 = v0 + v2; w1.normalize();
  SpatialVector w2 = v1 + v0; w2.normalize();

  SpatialVector
    v0_ = v0,
    v1_ = v1,
    v2_ = v2,
    w0_ = w0,
    w1_ = w1,
    w2_ = w2;

  if(true){
    if(isInside(v, v0, w2, w1)) {
      v1 = w2; v2 = w1; subTriangleIndex = 0;
    } else if(isInside(v, v1, w0, w2)) {
      v0 = v1; v1 = w0; v2 = w2; subTriangleIndex = 1;
    } else if(isInside(v, v2, w1, w0)) {
      v0 = v2; v1 = w1; v2 = w0; subTriangleIndex = 2;
    } else if(isInside(v, w0, w1, w2)) {
      v0 = w0; v1 = w1; v2 = w2; subTriangleIndex = 3;
    }
  }

  if(true && subTriangleIndex == -1){
    // cout << "SpatialIndex::subTriangleIndexByPoint WARNING TRYING BARYCENTRIC" << endl << flush;
    //		float64 lat,lon;
    //		v.getLatLonDegrees(lat, lon);
    //		cout << " v = " << v << ", lat,lon = " << lat << ", " << lon << endl << flush;
    bool verbose = false;
    if(isInsideBarycentric(v,v0,w2,w1,verbose)) {
      v1 = w2; v2 = w1; subTriangleIndex = 0;
    } else if(isInsideBarycentric(v,v1,w0,w2,verbose)) {
      v0 = v1; v1 = w0; v2 = w2; subTriangleIndex = 1;
    } else if(isInsideBarycentric(v,v2,w1,w0,verbose)) {
      v0 = v2; v1 = w1; v2 = w0; subTriangleIndex = 2;
    } else if(isInsideBarycentric(v,w0,w1,w2,verbose)) {
      v0 = w0; v1 = w1; v2 = w2; subTriangleIndex = 3;
    }
  }

  if( subTriangleIndex == -1  ) {
    // Okay, we're in trouble. A point which should be inside the triangle is not.
    // We assume that the point is in a subtriangle, though it may not appear so
    // because of floating point error. For example, consider points lying on edges--
    // for which case we would need to be more careful.

    float64 q0 = min_triangle_quality(v, v0, w2, w1);
    float64 q1 = min_triangle_quality(v, v1, w0, w2);
    float64 q2 = min_triangle_quality(v, v2, w1, w0);
    float64 q3 = min_triangle_quality(v, w0, w1, w2);
    float64 qmax = max(q0,max(q1,max(q2,q3)));
    if(q0 == qmax) {
      v1 = w2; v2 = w1; subTriangleIndex = 0;
    } else if(q1 == qmax) {
      v0 = v1; v1 = w0; v2 = w2; subTriangleIndex = 1;
    } else if(q2 == qmax) {
      v0 = v2; v1 = w1; v2 = w0; subTriangleIndex = 2;
    } else if(q3 == qmax) {
      v0 = w0; v1 = w1; v2 = w2; subTriangleIndex = 3;
    } else {
      throw SpatialFailure("SpatialIndex::subTriangleIndexByPoint: UNREACHABLE POINT");
    }

    // TODO Get a proper logger & properties system going.
    if(false) {
      //		cout << "SpatialIndex::subTriangleIndexByPoint WARNING NO GOOD TRIANGLE FOUND" << endl << flush;
      //		cout << "SpatialIndex::subTriangleIndexByPoint SUSPECT POINT LIES ON EDGE OR VERTEX" << endl << flush;
      //		cout << "SpatialIndex::subTriangleIndexByPoint ATTEMPTING BEST CHOICE ASSUMING POINT IN TRIXEL" << endl << flush;
      cout << "SpatialIndex::subTriangleIndexByPoint WARNING MAX-MIN TRIXEL QUALITY = " << qmax << endl << flush;
    }

    //		float64 lat,lon;

    // If the preceding fails, try the Barycentric approach. Should we warn?
    /*
      cout << "SpatialIndex::subTriangleIndexByPoint ERROR INDEX NOT FOUND " << endl
      << " v: " << v << endl
      << " v0: " << v0 << endl
      << " v1: " << v1 << endl
      << " v2: " << v2 << endl
      << " w0: " << w0 << endl
      << " w1: " << w1 << endl
      << " w2: " << w2 << endl << flush;
      cout << endl << flush;
      cout << "0 checkVectors " << endl; checkVectors(v,v0,w2,w1);
      cout << "1 checkVectors " << endl; checkVectors(v,v1,w0,w2);
      cout << "2 checkVectors " << endl; checkVectors(v,v2,w1,w0);
      cout << "3 checkVectors " << endl; checkVectors(v,w0,w1,w2);

      cout << "0 Barycentric test: " << isInsideBarycentric(v,v0,w2,w1) << endl << flush;
      cout << "1 Barycentric test: " << isInsideBarycentric(v,v1,w0,w2) << endl << flush;
      cout << "2 Barycentric test: " << isInsideBarycentric(v,v2,w1,w0) << endl << flush;
      cout << "3 Barycentric test: " << isInsideBarycentric(v,w0,w1,w2) << endl << flush;
    */

    /*
      bool verbose = true;
      if(isInsideBarycentric(v,v0,w2,w1,verbose)) {
      v1 = w2; v2 = w1; subTriangleIndex = 0;
      } else if(isInsideBarycentric(v,v1,w0,w2,verbose)) {
      v0 = v1; v1 = w0; v2 = w2; subTriangleIndex = 1;
      } else if(isInsideBarycentric(v,v2,w1,w0,verbose)) {
      v0 = v2; v1 = w1; v2 = w0; subTriangleIndex = 2;
      } else if(isInsideBarycentric(v,w0,w1,w2,verbose)) {
      v0 = w0; v1 = w1; v2 = w2; subTriangleIndex = 3;
      }

      // cout << "subTriangleIndex = " << subTriangleIndex << endl << flush;

      if(subTriangleIndex == -1) {
      // TODO If sub triangle index fails, maybe we should throw an exception instead...
      cout << "SpatialIndex::subTriangleIndexByPoint FAILED INDEX NOT FOUND" << endl << flush;
      float64 lat,lon;
      v.getLatLonDegrees(lat, lon);
      cout << " v = " << v << ", lat,lon = " << lat << ", " << lon << endl << flush;
      // cout << endl << "Exiting..." << endl << flush;
      // exit(1);
      throw SpatialFailure("SpatialIndex::subTriangleIndexByPoint: INDEX NOT FOUND");
      }
    */
  } /* else if(true) {
    // See if the Barycentric calculation matches the triple product.
    // TODO Determine if we should switch to the Barycentric calculation...

    int subTriangleIndexSave = subTriangleIndex;
    subTriangleIndex = -1;
    if(isInsideBarycentric(v,v0_,w2_,w1_)) {
    subTriangleIndex = 0;
    } else if(isInsideBarycentric(v,v1_,w0_,w2_)) {
    subTriangleIndex = 1;
    } else if(isInsideBarycentric(v,v2_,w1_,w0_)) {
    subTriangleIndex = 2;
    } else if(isInsideBarycentric(v,w0_,w1_,w2_)) {
    subTriangleIndex = 3;
    }
    if( subTriangleIndex < 0) {
    cout << "SpatialIndex::subTriangleIndexByPoint Barycentric Test Fails! Exiting!" << endl << flush;
    exit(1);
    } else if(subTriangleIndex != subTriangleIndexSave){
    cout << "SpatialIndex::subTriangleIndexByPoint Barycentric Test disagrees with Triple Product! Exiting!" << endl << flush;
    cout << " Barycentric: " << subTriangleIndex << ", Triple Product: " << subTriangleIndexSave << endl << flush;
    exit(1);
    }
    } */
  return subTriangleIndex;
}



void SpatialIndex::printNode(int nodeIndex) const {
  cout << "nIdx: " << nodeIndex << " "
       << " own-idx, numeric-id-name, parent, children "
       << N(nodeIndex).index_ << " "
       << N(nodeIndex).id_ << "=(x"
       << hex << N(nodeIndex).id_ << ") "
       << dec << N(nodeIndex).parent_ << " "
       << N(nodeIndex).childID_[0] << " "
       << N(nodeIndex).childID_[1] << " "
       << N(nodeIndex).childID_[2] << " "
       << N(nodeIndex).childID_[3] << " "
       << endl << flush;
}

/////////////CONSTRUCTOR//////////////////////////////////
//

/**
 * Construct a spatial index, an internal construct used by other interfaces.
 *
 * Key data structures created include
 * - \a layers_ [0..buildlevel] This contains information about the numbers of elements at each level.
 * - \a nodes_  [0..nNode_]     This contains the nodes, which include index, interconnection, and geometry information.
 *
 * Note that the zeroth node \c nodes_(0).index_=0 is the invalid node corresponding to htm-index == 0.
 *
 * Note \c buildlevel_ is set to maxlevel if buildlevel is 0.
 * Note \c buildlevel_ is set to maxlevel if buildlevel > maxlevel.
 * Note \c buildlevel_ is set to buildlevel otherwise. Then we construct indices on the fly.
 *
 * Calls \c newNodep, makeNewLayer, ...
 *
 * @param [in] maxlevel   The current level of focus for the index, the level of the leaves.
 * @param [in] buildlevel The level to which nodes are stored in nodes_.
 */

SpatialIndex::SpatialIndex(size_t maxlevel, size_t buildlevel, SpatialRotation rot) :
  maxlevel_(maxlevel),
  buildlevel_( (buildlevel == 0 || buildlevel > maxlevel) ? maxlevel : buildlevel),
  rot_(rot)
{
  //	debug = false;

  size_t nodes,vertices;

  vMax(&nodes,&vertices);
  layers_.resize(buildlevel_+1);	// allocate enough space already
  nodes_.resize(nodes+1); 			// allocate space for all nodes
  vertices_.resize(vertices+1); 	// allocate space for all vertices

  N(0).index_ = 0;					// initialize invalid node

  // TODO A better interface is possible by using layers. MLR
  // initialize first layer
  layers_[0].level_ = 0;
  layers_[0].nVert_ = 6;
  layers_[0].nNode_ = 8;
  layers_[0].nEdge_ = 12;
  layers_[0].firstIndex_ = 1;
  layers_[0].firstVertex_ = 0;

  // set the first 6 vertices // TODO Change for icosahedron.
  /*
    float64 v[6][3] = {
    {0.0L,  0.0L,  1.0L}, // 0 k
    {1.0L,  0.0L,  0.0L}, // 1 i
    {0.0L,  1.0L,  0.0L}, // 2 j
    {-1.0L,  0.0L,  0.0L}, // 3 -i
    {0.0L, -1.0L,  0.0L}, // 4 -j
    {0.0L,  0.0L, -1.0L}  // 5 -k
    };
  */

  SpatialVector
    xhat(1.0,0.0,0.0),
    yhat(0.0,1.0,0.0),
    zhat(0.0,0.0,1.0);

  SpatialVector body_xhat = rot.rotated_from(xhat);
  SpatialVector body_yhat = rot.rotated_from(yhat);
  SpatialVector body_zhat = rot.rotated_from(zhat);

  // float64 *v[6];
  std::vector<float64> v[6];
  v[0]=       body_zhat.toArray();
  v[1]=       body_xhat.toArray();
  v[2]=       body_yhat.toArray();
  v[3]=(-1.0*body_xhat).toArray();
  v[4]=(-1.0*body_yhat).toArray();
  v[5]=(-1.0*body_zhat).toArray();

  for(int i = 0; i < 6; i++)
    vertices_[i].set( v[i][0], v[i][1], v[i][2]);

  // for(int i=0; i<6; ++i) {
  //   delete [] v[i];
  // }

  // create the first 8 nodes - index 1 through 8
  index_ = 1;
  newNode(1,5,2,8,0);  // S0 = numericIdName 8 -- Note this is BitShifted HTM encoding
  newNode(2,5,3,9,0);  // S1 = numericIdName 9
  newNode(3,5,4,10,0); // S2 = numericIdName a
  newNode(4,5,1,11,0); // S3 = numericIdName b
  newNode(1,0,4,12,0); // N0  i k -j
  newNode(4,0,3,13,0); // N1 -j k -i
  newNode(3,0,2,14,0); // N2 -i k  j
  newNode(2,0,1,15,0); // N3  j k  i

  //  loop through buildlevel steps, and build the nodes for each layer

  size_t pl=0;
  size_t level = buildlevel_;
  while(level-- > 0) {
    SpatialEdge edge(*this, pl);
    edge.makeMidPoints();
    makeNewLayer(pl);
    ++pl;
  }

  sortIndex();

  //  for(int nodeIndex=0;nodeIndex<1000;nodeIndex++) {
  //	  printNode(nodeIndex);
  //  }
}

SpatialIndex::~SpatialIndex() {

}

/////////////SHOWVERTICES/////////////////////////////////
// showVertices: print every vertex to the output stream
void
SpatialIndex::showVertices(std::ostream & out) const
{
  for(size_t i = 0; i < vertices_.size()-1; i++)
    out << vertices_[i] << std::endl;
}

/////////////NODEVERTEX///////////////////////////////////
// nodeVertex: return index of vertices for a node
void 
SpatialIndex::nodeVertex(const size_t idx, 
			 size_t & v1, size_t & v2, size_t & v3) const {
  //	cout << "nodeVertex-getMemo" << endl << flush;
  v1 = nodes_[idx].v_[0];
  v2 = nodes_[idx].v_[1];
  v3 = nodes_[idx].v_[2];
}

/////////////NODEVERTEX///////////////////////////////////
// nodeVertex: return the vectors of the vertices, based on the ID
// // TODO Note:  Which ID?
// 
//void
//SpatialIndex::nodeVertex(const uint64 id,
//			 SpatialVector & v0,
//			 SpatialVector & v1,
//			 SpatialVector & v2) const {
//	nodeVertex(id,v0,v1,v2,false);
//}

/**
 *
 * NodeId64, aka nodeIndex, is htmId+IOFFSET for the depth of the current index.
 * If depth(NodeId64) == index.buildlevel_, then we just access nodes_.
 * If the depth is shallower, then we assume the caller knows what they are doing.
 * Usually such a nodeIndex (one at less depth) is found when traversing nodes_.
 * When going deeper than buildlevel_ we build the needed geometry as we go.
 *
 * Note that while the octagon is stored at nodes_[1..8] shallower nodes are stored
 * at the end of nodes_ in the reverse order that they were orginally created in
 * makeNewLayer.  Also note that nodes_ is sorted so that nodes at the depth
 * buildlevel_ are stored in nodes_[9..9+layers_[layer].nNode_-1] in ascending order.
 *
 * TODO What is the relationship between depth and layer?
 *
 * @param[in] nodeId64 (aka nodeIndex) is the index to the field nodes_.
 * @param[out] v0, v1, v2
 */
void
SpatialIndex::nodeVertex(
			 const uint64 nodeId64, // Note this is a  node-id, not an external htm index.
			 SpatialVector & v0, // These are the vertices of the triangle indexed by nodeId64.
			 SpatialVector & v1,
			 SpatialVector & v2
			 //			 ,bool verbose
			 ) const {

  // cout << "nodeVertex-100" << " ni64 0x" << setw(16) << setfill('0') << hex << nodeId64 << dec <<  endl << flush;

  //	cout << "buildlevel,maxlevel: " << buildlevel_ << ", " << maxlevel_ << " " << flush;
  if(buildlevel_ == maxlevel_) {
    // TODO Usually buildlevel_ != maxlevel_, so the following code is suspect.
    // TODO Note that the in the original code we use NodeId64 as an index into nodes_ itself.
    // TODO If this holds, then we've built and stored all of the nodes.
    // TODO Need to verify role of IOFFSET here...
    //	cout << "nodeVertex-1000-LookingUpVertices" << endl << flush;
    uint64 nodeId32 = (uint64)nodeId64; // Note no shifting or offsetting.
    if(maxlevel_ == 0) {
      nodeId32 -= (IOFFSET-1); // TODO Why me? It would be nice to analyze and verify this is correct.
    }
    //    cout << "nodeId32=" << nodeId32 << flush;
    //    cout << ", nnodes=" << nodes_.size() << flush;
    v0 = vertices_[nodes_[nodeId32].v_[0]];
    v1 = vertices_[nodes_[nodeId32].v_[1]];
    v2 = vertices_[nodes_[nodeId32].v_[2]];
    return;
  }

  uint64 sid = (nodeId64 - IOFFSET) >> ((maxlevel_ - buildlevel_)*2);
  uint64 nodeId32 = (uint64)( sid );

  // cout << "NOTE: building vertices..." << endl;

  // TODO What stored leaf are we in?  We should cut the extra off of id to get idx.
  // cout << "v0 " << flush;
  // cout << endl << flush;
  // cout << " buildlevel, maxlevel: " << buildlevel_ << ", " << maxlevel_ << endl << flush;
  // cout << " nodeId32, IOFFSET " << hex << nodeId32 << dec << " " << IOFFSET << endl << flush;
  // cout << " nodeId64, IOFFSET " << hex << nodeId64 << dec << endl << flush;
  v0 = vertices_[nodes_[nodeId32+IOFFSET].v_[0]];
  // cout << "v1 " << flush;
  v1 = vertices_[nodes_[nodeId32+IOFFSET].v_[1]];
  // cout << "v2 " << flush;
  v2 = vertices_[nodes_[nodeId32+IOFFSET].v_[2]];
  // cout << " done " << endl;

  // cout << "nv-200" << endl;

  // loop through additional levels,
  // pick the correct triangle accordingly, storing the
  // vertices in v1,v2,v3
  for(uint32 i = buildlevel_ + 1; i <= maxlevel_; i++) {
    // cout << "nv-210 " << i << endl;
    uint64 j = ( (nodeId64 - IOFFSET) >> ((maxlevel_ - i)*2) ) & 3;
    partitionTriangle(v0,v1,v2,j);
  }
  // cout << "nv-999" << endl;

}

/////////////MAKENEWLAYER/////////////////////////////////
/** makeNewLayer: generate a new layer and the nodes in it
 *
 * @param oldlayer
 */
void 
SpatialIndex::makeNewLayer(size_t oldlayer)
{
  uint64 index, numericIdName;
  size_t newlayer = oldlayer + 1;
  layers_[newlayer].level_  = layers_[oldlayer].level_+1;
  layers_[newlayer].nVert_  = layers_[oldlayer].nVert_ + 
    layers_[oldlayer].nEdge_;
  layers_[newlayer].nNode_  = 4 * layers_[oldlayer].nNode_;
  layers_[newlayer].nEdge_  = layers_[newlayer].nNode_ + 
    layers_[newlayer].nVert_ - 2;
  layers_[newlayer].firstIndex_ = index_;   
  layers_[newlayer].firstVertex_ = layers_[oldlayer].firstVertex_ + 
    layers_[oldlayer].nVert_;

  uint64 ioffset = layers_[oldlayer].firstIndex_ ; // ???

  //  cout
  //  	  << " level_, newlayer, " << layers_[newlayer].level_ << " " << newlayer
  //	  << " ioffset: " << ioffset
  //	  << " firstIndex_: " << " " << layers_[newlayer].firstIndex_
  //	  << endl << flush;

  for(index = ioffset;
      index < ioffset + layers_[oldlayer].nNode_; index++){
    numericIdName = N(index).id_ << 2;
    //    cout << " index,numericIdName: " << index << " " << numericIdName << flush;
    ICHILD(0) = newNode(IV(0),IW(2),IW(1),numericIdName++,index);
    ICHILD(1) = newNode(IV(1),IW(0),IW(2),numericIdName++,index);
    ICHILD(2) = newNode(IV(2),IW(1),IW(0),numericIdName++,index);
    ICHILD(3) = newNode(IW(0),IW(1),IW(2),numericIdName,index);
    //    cout << endl << flush;
  }

  layers_[newlayer].lastIndex_ = index_ - 1;

  //  cout << " lastIndex_: " << layers_[newlayer].lastIndex_ << endl << flush;

}

/////////////NEWNODE//////////////////////////////////////
/** newNode: make a new node initializing child and midpoints to zero.
 *
 * @param v1, v2, v3 vertex indices of the new node
 * @param numericIdName
 * @param parent
 * @return The index of the next node.
 */
uint64
SpatialIndex::newNode(size_t v1, size_t v2,size_t v3,uint64 numericIdName,uint64 parent)
{
  IV_(0) = v1;		// vertex indices
  IV_(1) = v2;
  IV_(2) = v3;

  IW_(0) = 0;		// middle point indices
  IW_(1) = 0;
  IW_(2) = 0;

  ICHILD_(0) = 0;	// child indices
  ICHILD_(1) = 0;	// index 0 is invalid node.
  ICHILD_(2) = 0;
  ICHILD_(3) = 0;

  N(index_).id_ = numericIdName;			// set the id mlr: the numeric-id-name
  N(index_).index_ = index_;	// set the index (before sorting)
  N(index_).parent_ = parent;	// set the parent

  return index_++;
}


float64
SpatialIndex::area(uint64 ID) const
{

  SpatialVector n0;
  SpatialVector n1;
  SpatialVector n2;

  nodeVertex(ID, n0, n1, n2);
  return area(n0,n1,n2);
}

/** Precompute the area (steradians) of a node using

    \f{align}{
    \text{area} &= 4\arctan\left(\sqrt{
    \tan\frac{s}{2}\,
    \tan\frac{s-a}{2}\,
    \tan\frac{s-b}{2}\,
    \tan\frac{s-c}{2}
    }\right)\\
    \mbox{where }&\\
    s &= \frac{1}{2}(a+b+c)
    \f}

    The angles between the vertices are denoted \f$a, b, c\f$.

    Many thanks to Eduard Masana, emasana@pchpc10.am.ub.es.

    * @param[in] v0, v1, v2  vertices of the spherical triangular area.
    * @return The area of the spherical triangle in steradians.
    *
    * TODO Need to validate vectors and throw if wrong.
    * TODO Consider a SpatialUnitVector class. Also, has someone done this already?
    */
float64
SpatialIndex::area(const SpatialVector & v0, 
		   const SpatialVector & v1,
		   const SpatialVector & v2) const {

  // SpatialVector u0 = v0; u0.normalize();
  // SpatialVector u1 = v1; u1.normalize();

  float64 a = acos( v0 * v1 );
  float64 b = acos( v1 * v2 );
  float64 c = acos( v2 * v0 );

  float64 s = (a + b + c)/2.0;

  // std::cout << "0*1, 1*2, 2*0: " << v0*v1 << " " << v1*v2 << " " << v2*v0 << std::endl;
  // std::cout << "a,b,c,s: " << " " << a << " " << b << " " << c << " " << s << std::endl;

  float64 area = 4.0*atan(sqrt(tan(s/2.0)*
			       tan((s-a)/2.0)*
			       tan((s-b)/2.0)*
			       tan((s-c)/2.0)));        
  return area;
}

/////////////VMAX/////////////////////////////////////////

/** Compute the following.
    -# The maximum number of vertices for the polyhedron after buildlevel subdivisions,
    -# the total number of nodes that we store, and
    -# calculate the number of leaf nodes that we eventually have.

    Set the following.
    - \p nodes to the total number of nodes (faces) out to buildlevel_
    - \p vertices to the number of thereof
    - \p storedleaves_ to the number of faces at buildlevel_
    - \p leaves_ to the number of faces at the deepest level we might search
    - \p leaves_ = \p numberOfFaces(max(buildlevel_,maxlevel_))

    * @param[in,out] nodes
    * @param[in,out] vertices
    */
void
SpatialIndex::vMax(size_t *nodes, size_t *vertices) {
  /// Currently use the octagon case to set the initial values.
  uint64 numberOfVertices = 6;
  uint64 numberOfEdges    = 12;
  uint64 numberOfFaces    = 8;
  int32 i  = buildlevel_; /// Count through the number of levels we build.
  *nodes = (size_t)numberOfFaces;
  while(i-->0){
    numberOfVertices += numberOfEdges;
    numberOfFaces *= 4;
    numberOfEdges  = numberOfFaces + numberOfVertices -2;
    *nodes += (size_t)numberOfFaces;  }
  *vertices = (size_t)numberOfVertices;
  storedleaves_ = numberOfFaces;
  /// Calculate number of leaves.
  i = maxlevel_ - buildlevel_;  /// Recall \p maxlevel_-buildlevel_ is how deep beyond the stored nodes we go.
  while(i-- > 0)
    numberOfFaces *= 4;
  leaves_ = numberOfFaces;
  //  dbg
  // cout << "vMax: leaves_, storedleaves_ " << leaves_ << " " << storedleaves_ << endl << flush;
}

/////////////SORTINDEX////////////////////////////////////
// sortIndex: sort the index so that the first node is the invalid node
//            (index 0), the next 8 nodes are the root nodes
//            and then we put all the leaf nodes in the following block
//            in ascending id-order.
//            All the rest of the nodes is at the end.
void
SpatialIndex::sortIndex() {
  ValueVectorQuad oldnodes(nodes_); // create a copy of the node list
  size_t index;
  size_t nonleaf;
  size_t leaf;

#define ON(x) oldnodes[(x)]

  // now refill the nodes_ list according to our sorting.
  for( index=IOFFSET, leaf=IOFFSET, nonleaf=nodes_.size()-1;
       index < nodes_.size(); index++) {

    if( ON(index).childID_[0] == 0 ) { // childnode
      // set leaf into list
      N(leaf) = ON(index);
      // set parent's pointer to this leaf
      for (size_t i = 0; i < 4; i++) {
	if(N(N(leaf).parent_).childID_[i] == index) {
	  N(N(leaf).parent_).childID_[i] = leaf;
	  break;
	}
      }
      leaf++;
    } else {
      // set nonleaf into list from the end
      // set parent of the children already to this
      // index, they come later in the list.
      N(nonleaf) = ON(index);
      ON(N(nonleaf).childID_[0]).parent_ = nonleaf;
      ON(N(nonleaf).childID_[1]).parent_ = nonleaf;
      ON(N(nonleaf).childID_[2]).parent_ = nonleaf;
      ON(N(nonleaf).childID_[3]).parent_ = nonleaf;
      // set parent's pointer to this leaf
      for (size_t i = 0; i < 4; i++) {
	if(N(N(nonleaf).parent_).childID_[i] == index) {
	  N(N(nonleaf).parent_).childID_[i] = nonleaf;
	  break;
	}
      }
      nonleaf--;
    }
  }
}

// TODO Review IDBYNAME -- uses uint32 works only to 15 levels.  Can we go to 64 easily?

//////////////////IDBYNAME/////////////////////////////////////////////////
/**
 *
 Construct the BitShifted encoded version from the string encoded version of the HTM.

 The BitShifted encoded version is also called the numericIdName, though that could have
 different encodings.

 Note:  This was "Translate ascii leaf name to a uint32," but we have now
 translated this to uint64 and have found no problems so far.

 The following encoding is used:

 The string leaf name has the always the same structure, it begins with
 an N or S, indicating north or south cap and then numbers 0-3 follow
 indicating which child to descend into. So for a level-5-index
 (deprecated terminology: depth-5-index) we have strings like

 N012023  S000222  N102302  etc

 Each of the numbers correspond to 2 bits of code (00 01 10 11) in the
 uint64. The first two bits are 10 for S and 11 for N. For example

 S0001122334455     depthSet//Level
 N 0 1 2 0 2 3     ASCII representation
 11000110001011  =  12683 (dec)
 1000110001011  =   4491 (stripped of depthSet bit)

 The leading bits are always 0.

 The first two characters, e.g. "S1", of the symbolic name are level 0. Every
 succeeding character corresponds to the next level of resolution.

 The number of triangles at a given level is 8*4**level.

 NOTE: This returns an "external" HTM index, with NO OFFESTS OR OTHER INTERNAL INDEXING.

 - Legacy 32-bit information:
 -- WARNING: This works only up to 15 levels.
 (we probably never need more than 7)
 - Converted to 64 bits. Checked to level 27. MLR 2016-01-08

 TODO factor out the part for name encoding.

 @param [in] name
 @return HTM index including leading depth bit.
*/
uint64
SpatialIndex::idByName(const string name) {
  // SpatialIndex::idByName(const char *name) {
  // Return the external HTM index. NO OFFSETS OR OTHER INDEXING
  //	cout << "idByName-1000" << endl << flush;
  uint64 out=0, i;
  uint32 size = 0;
  //	if(name == 0)              // null pointer-name
  if(name.empty())              // null pointer-name
    throw SpatialFailure("SpatialIndex:idByName:no name given");
  if(name[0] != 'N' && name[0] != 'S')  // invalid name
    throw SpatialFailure("SpatialIndex:idByName:invalid name",name.c_str());
  // size = strlen(name);       // determine string length
  size = name.length();
  /* TODO Note that size contains depth information, but isn't really used beyond
     sanity checking.
  */
  // at least size-2 required, don't exceed max 
  if(size < 2)
    throw SpatialFailure("SpatialIndex:idByName:invalid name - too short ",name.c_str());
  if(size > HTMNAMEMAX)
    throw SpatialFailure("SpatialIndex:idByName:invalid name - too long ",name.c_str());

  for(i = size-1; i > 0; i--) {// set bits starting from the end
    if(name[i] > '3' || name[i] < '0') // invalid name
      throw SpatialFailure("SpatialIndex:idByName:invalid name digit ",name.c_str());
    out += (uint64(name[i]-'0') << 2*(size - i -1));
  }
  i = 2;                     // set first pair of bits, first bit always set
  if(name[0]=='N') i++;      // for north set second bit too
  out += (i << (2*size - 2) );
  return out;
}

//////////////////NAMEBYID/////////////////////////////////////////////////
// Translate uint32 to an ascii leaf name
//
// The encoding described above may be decoded again using the following
// procedure:
//
//  * Traverse the uint32 from left to right.
//  * Find the first 'true' bit.
//  * The first pair gives N (11) or S (10).
//  * The subsequent bit-pairs give the numbers 0-3.
//

//char *
std::string
SpatialIndex::nameById(uint64 id){ // char * name){
  // SpatialIndex::nameById(uint64 id, std::string name){ // char * name){

  uint32 size=0, i;
#ifdef _WIN32
  uint64 IDHIGHBIT = 1;
  uint64 IDHIGHBIT2= 1;
  IDHIGHBIT = IDHIGHBIT << 63;
  IDHIGHBIT2 = IDHIGHBIT2 << 62;
#endif

  /*************
   // This code might be useful for hashing later !!

   // calculate the level (i.e. 8*4^level) and add it to the id:
  uint32 level=0, level4=8, offset=8;
  while(id >= offset) {
    if(++level > 13) { ok = false; offset = 0; break; }// level too deep
    level4 *= 4;
    offset += level4;
  }
  id += 2 * level4 - offset;
  **************/

  // determine index of first set bit
  for(i = 0; i < IDSIZE; i+=2) {
    if ( (id << i) & IDHIGHBIT ) break;
    if ( (id << i) & IDHIGHBIT2 ) { // invalid id
      cout << hex;
      cout << i << endl;
      cout << id << endl;
      cout << (id << i) << endl;
      cout << IDHIGHBIT2 << endl;
      cout << dec;
      cout << "failure at id=" << id << endl << flush;
      throw SpatialFailure("SpatialIndex:nameById: invalid ID id at IHIGHBIT2");
    }
  }
  if(id == 0)
    throw SpatialFailure("SpatialIndex:nameById: invalid ID id==0");

  size=(IDSIZE-i) >> 1;
  // allocate characters
  //  if(!name.empty())
  std::string name(size+1,' '); //  = std::string(size+1,' ');
  // name = new char[size+1];

  // fill characters starting with the last one
  for(i = 0; i < size-1; i++)
    name[size-i-1] = '0' + char( (id >> i*2) & 3);

  // put in first character
  if( (id >> (size*2-2)) & 1 ) {
    name[0] = 'N';
  } else {
    name[0] = 'S';
  }
  // name[size] = 0; // end string
  name.erase(size,std::string::npos);

  return name;
}
//////////////////POINTBYID////////////////////////////////////////////////
// Find a vector for the leaf node given by its ID
//

/**
 * Return the "center" of the triangle
 * @param vec output the center of the triangle as a SpatialVector
 * @param htmId The external numeric-id of the triangle
 */
void
SpatialIndex::pointByHtmId(SpatialVector &vec, uint64 htmId) const {
  // TODO Check out index->nodeVertex for a possibly correct way to get the right point.
  // TODO The way done below may depend on the bitlist and not correctly handle the dynamically created "leaf" index.
  // uint64 leafID = this->leafNumberById(htmId)+IOFFSET;
  // this->pointById(vec,leafID);
  // LAST WORKING ORIG  this->pointById(vec,NodeID64FromHtmId(htmId));


  // cout << 1000
  //      << " htmId 0x" << setw(16) << setfill('0') << hex << htmId << dec << endl << flush;
  int64 id64 = NodeID64FromHtmId(htmId);
  // cout << 1100
  //     << " id64  0x" << setw(16) << setfill('0') << hex << id64 << dec << endl << flush;
  this->pointById(vec,id64);
  // cout << 1200 << endl << flush;
}

void
SpatialIndex::nodeVertexByHtmId(SpatialVector &v1, SpatialVector &v2, SpatialVector &v3, uint64 htmId) const {
  // uint64 leafID = this->leafNumberById(htmId)+IOFFSET;
  // this->nodeVertex(leafID,v1,v2,v3); // leafID is a nodeID64
  this->nodeVertex(NodeID64FromHtmId(htmId),v1,v2,v3);
}

float64 SpatialIndex::areaByHtmId(uint64 htmId) const {
  // uint64 leafID = this->leafNumberById(htmId)+IOFFSET;
  // return this->area(leafID);
  return this->area(NodeID64FromHtmId(htmId));
}
/**
 * Return a leafID, i.e. a NodeID64 from an HtmId.
 */
uint64 SpatialIndex::NodeID64FromHtmId(uint64 htmId) const {
  return this->leafNumberById(htmId)+IOFFSET;
}

/**
 * Return the vector sum of the vertices normalized by its length.
 * @param vec SpatialVector center of triangle returned
 * @param nodeId64 the internal node-id of the triangle
 */
void
SpatialIndex::pointById(SpatialVector &vec, uint64 nodeId64) const {
  float64 center_x, center_y, center_z, sum;
  // char name[HTMNAMEMAX];
  SpatialVector v0, v1, v2; //
  // TODO nodeVertex is expecting something like a leafId
  // TODO nodeVertex might not account for an offset.
  // cout << 2000 << endl << flush;
  this->nodeVertex(nodeId64, v0, v1, v2);
  // cout << 2100 << endl << flush;	
  //    cerr << "---------- Point by id: " << name << endl;
  //	v0.show(); v1.show(); v2.show();
  center_x = v0.x_ + v1.x_ + v2.x_;
  center_y = v0.y_ + v1.y_ + v2.y_;
  center_z = v0.z_ + v1.z_ + v2.z_;
  sum = center_x * center_x + center_y * center_y + center_z * center_z;
  sum = sqrt(sum);
  center_x /= sum;
  center_y /= sum;
  center_z /= sum;
  vec.x_ = center_x;
  vec.y_ = center_y;
  vec.z_ = center_z; // I don't want it normalized or radec to be set,
  //	cerr << " - - - - " << endl;
  //	vec.show();
  //	cerr << "---------- Point by id Retuning" << endl;
}

/**
 * Do the geometric calculation to find neighboring triangles across the faces.
 * Not as cheap if siblings are known. The ith neighbor is across the face
 * opposite the ith vertex as returned from nodeVertex.
 * @param neighbors
 * @param htmId
 */
void
SpatialIndex::NeighborsAcrossEdgesFromHtmId(
					    uint64 neighbors[3],
					    uint64 htmId,
					    SpatialVector workspace[18]
					    ) const {
  SpatialVector v1, v2, v3, m12, m23, m13, q1, q2, q3;
  uint64 nodeId = nodeIndexFromId(htmId);
  nodeVertex(nodeId,v1,v2,v3);
  //	SpatialVector center = pointByHtmId(htmId);
  SpatialVector center = v1 + v2 + v3; center.normalize();
  m12 = v1 + v2; m12.normalize();
  m23 = v2 + v3; m23.normalize();
  m13 = v1 + v3; m13.normalize();
  // float64 alpha = 1.1;
  float64 alpha = 1.25;
  // float64 alpha = 1.5;
  SpatialVector centerAlpha = center * ( 1-alpha );
  q1 = centerAlpha + m23 * alpha; q1.normalize();
  q2 = centerAlpha + m13 * alpha; // q2.normalize();
  q3 = centerAlpha + m12 * alpha; // q3.normalize();
  neighbors[0] = idByPoint(q1);
  neighbors[1] = idByPoint(q2);
  neighbors[2] = idByPoint(q3);

  //	cout << "c  " << center << endl << flush;
  //	cout << "m12 " << m12 << endl << flush;
  //	cout << "m23 " << m23 << endl << flush;
  //	cout << "m13 " << m13 << endl << flush;
  //	cout << "q1 " << q1 << endl << flush;
  //	cout << "q2 " << q2 << endl << flush;
  /*
    cout << "." << endl << flush;
    cout << "htmId   " << center << " - 0x" << hex << htmId << dec << endl << flush;
    cout << "q1 edge " << q1 << " - 0x" << hex << neighbors[0] << dec << endl << flush;
    cout << "q2 edge " << q2 << " - 0x" << hex << neighbors[1] << dec << endl << flush;
    cout << "q3 edge " << q3 << " - 0x" << hex << neighbors[2] << dec << endl << flush;
  */
  int j = 0;
  workspace[j++] = v1;
  workspace[j++] = v2;
  workspace[j++] = v3;
  workspace[j++] = m12;
  workspace[j++] = m13;
  workspace[j++] = m23;
  workspace[j++] = q1;
  workspace[j++] = q2;
  workspace[j++] = q3;
}

/**
 * Calculate the neighboring triangles that are across the vertices.
 * The neighbors are in groups of 3 ordered counterclockwise as the vertices are
 * returned from nodeVertex.
 * @param neighbors
 * @param htmId
 */
void
SpatialIndex::NeighborsAcrossVerticesFromEdges(
					       uint64 neighbors[9],
					       uint64 neighbors_edge[3],
					       uint64 htmId,
					       SpatialVector workspace[18]
					       ) const {
  SpatialVector v1, v2, v3, m12, m23, m13;
  SpatialVector q0, q1, q2, q3, q4, q5, q6, q7, q8;
#if 0
  uint64 nodeId = nodeIndexFromId(htmId);
#endif
  // See NeighborsAcrossEdgesFromHtmId.
  int jw = 0;
  v1  = workspace[jw++];
  v2  = workspace[jw++];
  v3  = workspace[jw++];
  m12 = workspace[jw++];
  m13 = workspace[jw++];
  m23 = workspace[jw++];

  SpatialVector center = v1 + v2 + v3; center.normalize();
  /*
    m12 = v1 + v2; m12.normalize();
    m23 = v2 + v3; m23.normalize();
    m13 = v1 + v3; m13.normalize();
  */

  // float64 alpha = 1.1;
  // float64 beta  = 0.1;

  float64 alpha = 1.25;
  float64 beta  = 0.25;

  // float64 alpha = 1.5;
  // float64 beta =  0.5;

  // float64 alpha = 1.001;
  // float64 beta =  0.001;

  SpatialVector centerAlpha = center * ( 1-alpha );

  q0 = v1 + (m13-center) * beta; //q0.normalize();
  q1 = centerAlpha + v1 * alpha; //q1.normalize();
  q2 = v1 + (m12-center) * beta; //q2.normalize();

  q3 = v2 + (m12-center) * beta; //q3.normalize();
  q4 = centerAlpha + v2 * alpha; //q4.normalize();
  q5 = v2 + (m23-center) * beta; //q5.normalize();

  q6 = v3 + (m23-center) * beta; //q6.normalize();
  q7 = centerAlpha + v3 * alpha; //q7.normalize();
  q8 = v3 + (m13-center) * beta; //q8.normalize();

  neighbors[0] = idByPoint(q0);
  neighbors[1] = idByPoint(q1);
  neighbors[2] = idByPoint(q2);
  neighbors[3] = idByPoint(q3);
  neighbors[4] = idByPoint(q4);
  neighbors[5] = idByPoint(q5);
  neighbors[6] = idByPoint(q6);
  neighbors[7] = idByPoint(q7);
  neighbors[8] = idByPoint(q8);

  for( int iv=0; iv<9; ++iv ) {
    for( int ie=0; ie<3; ++ ie) {
      if( neighbors[iv] == neighbors_edge[ie] ) {
	// Look around for the correct neighbor.
	uint64 neighbors_[3]; SpatialVector workspace_[18];
	NeighborsAcrossEdgesFromHtmId(neighbors_,neighbors[iv],workspace_);
	int j=0; bool found=false;
	while(not found && j < 3) {
	  uint64 test_id = neighbors_[j];
	  if( test_id != htmId ) {
	    int k=0; bool k_found=false;
	    while( not k_found && k < 9 ) {
	      if( test_id == neighbors[k] ) {
		k_found = true;
	      } else {
		++k;
	      }
	    }
	    if( not k_found ) {
	      found = true;
	      neighbors[iv] = test_id;
	    }
	  }
	  ++j;
	}
	// TODO if not found, fail silently.
	cout << "SpatialIndex::NeighborsAcrossVerticesFromEdges::ERROR neighbor not found!!!" << endl << flush;
      }
    }
  }


  //	cout << "c  " << center << endl << flush;
  //	cout << "m12 " << m12 << endl << flush;
  //	cout << "m23 " << m23 << endl << flush;
  //	cout << "m13 " << m13 << endl << flush;
  //	cout << "q1 " << q1 << endl << flush;
  //	cout << "q2 " << q2 << endl << flush;
  //	cout << "q3 " << q3 << endl << flush;
  /*
    cout << "q0 " << q0 << " - 0x" << hex << neighbors[0] << dec << endl << flush;
    cout << "q1 " << q1 << " - 0x" << hex << neighbors[1] << dec << endl << flush;
    cout << "q2 " << q2 << " - 0x" << hex << neighbors[2] << dec << endl << flush;
    cout << "q3 vert " << q3 << " - 0x" << hex << neighbors[3] << dec << endl << flush;
    cout << "q4 " << q4 << " - 0x" << hex << neighbors[4] << dec << endl << flush;
    cout << "q5 " << q5 << " - 0x" << hex << neighbors[5] << dec << endl << flush;
    cout << "q6 " << q6 << " - 0x" << hex << neighbors[6] << dec << endl << flush;
    cout << "q7 " << q7 << " - 0x" << hex << neighbors[7] << dec << endl << flush;
    cout << "q8 " << q8 << " - 0x" << hex << neighbors[8] << dec << endl << flush;
  */

  //	int j = 0;
  //	workspace[j++] = v1;
  //	workspace[j++] = v2;
  //	workspace[j++] = v3;
  //	workspace[j++] = m12;
  //	workspace[j++] = m13;
  //	workspace[j++] = m23;
  //	workspace[j++] = q0;
  //	workspace[j++] = q1;
  //	workspace[j++] = q2;
  jw+=3; // Skip past the edge neighbors
  // Add the vertex neighbor guesses to the end of the workspace
  workspace[jw++] = q0;
  workspace[jw++] = q1;
  workspace[jw++] = q2;
  workspace[jw++] = q3;
  workspace[jw++] = q4;
  workspace[jw++] = q5;
  workspace[jw++] = q6;
  workspace[jw++] = q7;
  workspace[jw++] = q8;
}

#ifndef DIAG
#define DIAG1(expr)
#define DIAGOUT2(out,expr)
#define DIAGOUTDELTA(out,a,b)
#else
#define DIAG1(expr) expr;
#define DIAGOUT2(out,expr) out << expr;
#define DIAGOUTDELTA(out,a,b) {SpatialVector delta_ = a-b; cout << delta_.length() << " ";}
#endif

//////////////////IDBYPOINT////////////////////////////////////////////////
/** Find a leaf node where a vector points to
 *
 * Previous versions only worked when maxlevel_ was greater than buildlevel_.
 * TODO Determine if maxlevel_ must be greater than buildlevel_ (elsewhere).
 * Recall SpatialIndex(idMaxLevel,buildlevel_)
 * buildlevel_ tells you how deep the index was explicitly built.
 * maxlevel_ is the depth to which we want to enable searches
 *
 * The inverse of idByPoint is pointByHtmId, not pointById.
 *
 * @param v The input point whose enclosing triangle we seek
 * @return ID the HTM (external numeric-id) ID
 */
uint64
SpatialIndex::idByPoint(SpatialVector & v) const {
  uint64 index = 0;
  uint64 ID = 0;

  bool verbose = false;
  bool nudge = false;

  /**/
  // start with the 8 root triangles, find the one which v points to
  for(index=1; index <=8; index++) {
    if(isInsideBarycentric(v,V(0),V(1),V(2),verbose)) break;
  }
  DIAGOUT2(cout,"0 preamble i: " << index << endl << flush;);
  nudge = index == 9;

  // loop through matching child until leaves are reached
  while(ICHILD(0)!=0) {
    uint64 oldindex = index;
    for(size_t i = 0; i < 4; i++) {
      index = nodes_[oldindex].childID_[i];
      if(isInsideBarycentric(v,V(0),V(1),V(2),verbose)) break;
    }
  }
  DIAGOUT2(cout,"1 preamble i: " << index << endl << flush;);
  int index_barycentric = index;
  /**/

  // if(index == 9) {

  /* THE OLD WAY
   * TODO Why keep the legacy point-finding behavior? Right thing to do would be to examine where barycentric and old way differ.
   */

  SpatialVector dc;

  float64 dcs[8];
  for(index=1; index <=8; index++) {
    dc = V(0)+V(1)+V(2); dc.normalize(); dc = dc - v;
    dcs[index-1] = dc.length();
    DIAGOUT2(cout,dec << index << " index,dc " << setprecision(16) << dcs[index-1] << endl << flush;);
  }

  int     index_dcs_sort[8] = { 0 };
  index_dcs_sort[0]=0;

  for(int i = 1; i < 8; ++i) {
    int icmp = 0;
    while( icmp < i && (dcs[i] >= dcs[index_dcs_sort[icmp]]) ) {++icmp; }
    if( icmp <= i) {
      for( int j=i+1; j>=icmp; --j ) {
	index_dcs_sort[j]=index_dcs_sort[j-1];
      }
    }
    index_dcs_sort[icmp] = i;
  }
  DIAG1(for(int i=0;i<8;++i) {DIAGOUT2(cout,i << " i,sort,dcs " << index_dcs_sort[i] << " " << dcs[index_dcs_sort[i]] << endl << flush;);});

  // start with the 8 root triangles, find the one which v points to
  for(index=1; index <=8; index++) {
    if( (V(0) ^ V(1)) * v < -gEpsilon) continue;
    if( (V(1) ^ V(2)) * v < -gEpsilon) continue;
    if( (V(2) ^ V(0)) * v < -gEpsilon) continue;
    break;
  }

  if(nudge) {
    // if(false) {
    float64 epsilon = 1.0e-13; // TODO inject this dependency, expose as environment variable
    // float64 epsilon = 1.0e-17; // TODO inject this dependency, expose as environment variable
    SpatialVector ctr = V(0)+V(1)+V(2); ctr.normalize();
    v = (1-epsilon)*v + epsilon*ctr; v.normalize();
    for(index=1; index <=8; index++) {
      if( (V(0) ^ V(1)) * v < -gEpsilon) continue;
      if( (V(1) ^ V(2)) * v < -gEpsilon) continue;
      if( (V(2) ^ V(0)) * v < -gEpsilon) continue;
      break;
    }
  }

  DIAGOUT2(cout,dec << "index " << index << endl << flush;);
  float64 dc_start = -999;
  float64 dc_end   = -999;
  float64 dc_improvement = 2.0;
  int attempt = 0;
  int index_tried = index;
  int index_last_found = index;
  int itry = 0;
  do {
    ++attempt;
    if(attempt == 9) {
      stringstream ss;
      float64 lat=0,lon=0;
      v.getLatLonDegrees(lat, lon);
      ss << setprecision(16);
      ss << "SpatialIndex::idByPoint(sv): Lost Point Failure 1. No convergence."
	 << " point = " << v
	 << " index_barycentric = " << index_barycentric
	 << " index_last_found = " << index_last_found
	 << " latlon = " <<  lat << "," << lon
	 << " nudge " << ( nudge ? "true" : "false" )
	 << " dc_improvement = " << dc_improvement
	 << " dc_start = " << dc_start
	 << " dc_end = " << dc_end
	 << endl << flush;
      throw SpatialFailure(ss.str().c_str());
    }
    if(attempt>1) {
      index = index_dcs_sort[itry]+1;
      if( index == index_tried ) {
	++itry;
	if(itry == 8) {
	  stringstream ss;
	  float64 lat=0,lon=0;
	  v.getLatLonDegrees(lat, lon);
	  ss << setprecision(16);
	  ss << "SpatialIndex::idByPoint(sv): Lost Point Failure 2. No convergence."
	     << " point = " << v
	     << " index_barycentric = " << index_barycentric
	     << " index_last_found = " << index_last_found
	     << " latlon = " <<  lat << "," << lon
	     << " nudge " << ( nudge ? "true" : "false" )
	     << " dc_improvement = " << dc_improvement
	     << " dc_start = " << dc_start
	     << " dc_end = " << dc_end
	     << endl << flush;
	  throw SpatialFailure(ss.str().c_str());
	}
	index = index_dcs_sort[itry]+1;
      }
      ++itry;
      index_tried = index;
    }
    dc_start = dcs[index-1];
    DIAG1(cout << attempt << " trying " << index << endl << flush;);
    // loop through matching child until leaves are reached
#if 0
    int k = 1;
#endif
    while(ICHILD(0)!=0) {
      uint64 oldindex = index;
      for(size_t i = 0; i < 4; i++) {
	index = nodes_[oldindex].childID_[i];
	if( (V(0) ^ V(1)) * v < -gEpsilon) continue;
	if( (V(1) ^ V(2)) * v < -gEpsilon) continue;
	if( (V(2) ^ V(0)) * v < -gEpsilon) continue;
	break;
      }
      dc = V(0)+V(1)+V(2); dc.normalize(); dc = dc - v;
      DIAG1(cout << dec << k++ << " k,dc " << setprecision(16) << dc.length() << " i: " << index << endl << flush;);
    }
    dc = V(0)+V(1)+V(2); dc.normalize(); dc = dc - v;
    dc_end = dc.length();
    dc_improvement = dc_end/dc_start;
    DIAG1(cout << "dc start, end, ratio: " << dc_start << " " << dc_end << " " << dc_improvement << endl << flush;);
    index_last_found = index;
  } while ( dc_improvement > 0.125 && dc_end > 0.25 && !nudge );
  // }
  /**/

  // what if we haven't gotten to build level?
  // cout << "idbp: 100 " << endl << flush;
  // return if we have reached maxlevel
  if(maxlevel_ == buildlevel_)return N(index).id_;

  // cout << "idbp: 120 " << endl << flush;

  // from now on, continue to build name dynamically.
  // until maxlevel_ levels depth, continue to append the
  // correct index, build the index on the fly.
  // ORIG MLR 2019-1213
  // char name[HTMNAMEMAX];
  // nameById(N(index).id_,name);  //
  std::string name = nameById(N(index).id_);
  //

  //
  size_t len = name.length();
  // size_t len = strlen(name);
  // cout << "idbp: 180 name = '" << name << "', len = " << len << endl << flush;
  //
  SpatialVector v0 = V(0);
  SpatialVector v1 = V(1);
  SpatialVector v2 = V(2);

  /*  cout << "idbp: 200 " << endl << flush;
   cout << "idbp: 201 ml_ = " << maxlevel_ << endl << flush;
   cout << "idbp: 202 bl_ = " << buildlevel_ << endl << flush;
   cout << "idbp: 203 m-b = " << (int)maxlevel_ - (int)buildlevel_ << endl << flush; */

  // BitShifted
  // size_t level = maxlevel_ - buildlevel_;
  int level = maxlevel_ - buildlevel_;

  // cout << "idbp: 300 level = maxlevel_-buildlevel_ = " << level << endl << flush;

  // TODO make this whole routine less ad-hoc
  SpatialVector delta;
  DIAGOUT2(cout,endl << flush);
  if(level>0) {
#if 0
      int level0 = buildlevel_;
#endif
    while(level--) {
      DIAGOUT2(cout,setprecision(16) << dec;);
      // cout << level << " level,v... " << v << " " << v0 << " " << v1 << " " << v2 << endl << flush;
      DIAGOUT2(cout,level << " level,d0,d1,d2 ";);
      DIAGOUTDELTA(cout,v,v0);
      DIAGOUTDELTA(cout,v,v1);
      DIAGOUTDELTA(cout,v,v2);
      DIAGOUT2(cout,endl << flush;);
      int subTriangleIndex = subTriangleIndexByPoint(v,v0,v1,v2);
      // cout << "idbp: 350 level = " << level << ", level0 = " << level0++ << ", subTriangleIndex = " << subTriangleIndex << ", name = " << name;
      // ORIG MLR 2019-1215 name[len++] = '0'+char(subTriangleIndex);
      name = name + (char)('0'+char(subTriangleIndex));
      // cout << ", name = '" << name << "'" << endl << flush;
    }
  } else {
    len = len + level;
    if(len<2) {
      throw SpatialFailure("SpatialIndex::idByPoint::LevelTooLow len < 2");
    }
    name.erase(len,std::string::npos);
  }

  DIAGOUT2(cout,level << " level,d0,d1,d2 ";);
  DIAGOUTDELTA(cout,v,v0);
  DIAGOUTDELTA(cout,v,v1);
  DIAGOUTDELTA(cout,v,v2);
  DIAGOUT2(cout,endl << flush << level << " level,v... " << v << " " << v0 << " " << v1 << " " << v2 << endl << flush;);

  // ORIG MLR 2019-1215 name[len] = '\0';
  // if(name.length() > 1+len) { name.erase(len,std::string::npos); }

  // cout << "idbp: 400 name = '" << name << "', len = " << len << ", name.length=" << name.length() << endl << flush;

  ID = idByName(name); // TODO Didn't we just calculate the bits above?

  //    cout
  //	<< "name: " << name
  //	<< " ID " << ID << endl << flush;

  //	cout << "idbp: 500 " << endl << flush;

  if(0){
    SpatialVector vec;
    pointById(vec, ID);
    //cerr << "pointById: ----------------" << endl;
    //vec.show();
  }    
  //	cout << "idbp: 600 " << endl << flush;
  return ID;
}

#undef DIAG
#undef DIAG1
#undef DIAGOUT2
#undef DIAGOUTDELTA

uint64 SpatialIndex::indexAtNodeIndex(uint64 nodeIndex) {
  return nodes_[nodeIndex].index_;
}
uint64 SpatialIndex::idAtNodeIndex(uint64 nodeIndex) {
  return nodes_[nodeIndex].id_;
}
uint64 SpatialIndex::layersSize() const{
  return layers_.size();
}
uint64 SpatialIndex::firstIndexOfLayerAtDepth(uint64 depth){
  cout << "SpatialIndex::firstIndexOfLayerAtDepth: Warning!!! Deprecated!!!" << endl << flush;
  return layers_[depth].firstIndex_;
}
uint64 SpatialIndex::firstIndexOfLayerAtLevel(uint64 level){
  return layers_[level].firstIndex_;
}

int depthOfName(const char name[]) { return strlen(name)-1; }
int levelOfName(const char name[]) { return strlen(name)-2; } // TODO Move to NameEncoding.
int levelOfDepth(int depth) { return depth-1; } // TODO Remove.

int depthOfId(uint64 htmId) {
  return levelOfId(htmId) + 1;
}
/** Deprecated hardcoded way to find level of an htmID
 *
 * TODO replace with NameEncoding
 *
 * @param htmId
 * @return level of htmID
 */
int levelOfId(uint64 htmId) {
  int i;
  uint32 size;
  // cout << "loid: " << setw(16) << setfill('0') << hex << htmId << dec << endl << flush;
  // determine index of first set bit
  for(i = 0; i < IDSIZE; i+=2) {
    if ( (htmId << i) & IDHIGHBIT ) break;
    if ( (htmId << i) & IDHIGHBIT2 )  // invalid id
      throw SpatialFailure("SpatialIndex:nameById: invalid ID 1");
  }
  if(htmId == 0)
    throw SpatialFailure("SpatialIndex:nameById: invalid ID 2");
  size=(IDSIZE-i) >> 1;
  return size-2;
}

/** Deprecated hardcoded way to find the nodeIndex from an htmId
 * TODO replace with NameEncoding
 * @param id contains the htmId (external numeric-id) for a triangle
 * @return that triangle's nodeIndex
 */
uint64 SpatialIndex::nodeIndexFromId(uint64 id) const {
  // This nodeIndex is only valid if depth == maxlevel+1
  // We could fix this to go to the non-leaf parts of the nodes_ array/index.
  int depth = depthOfId(id);
  if (depth != maxlevel_+1) {
    cout << "si:nifi: id=" << hex << id << dec << " maxlevel_=" << maxlevel_ << " depth=" << depth << endl << flush;
    return 0;  // TODO Make this throw an exception?
  }
  //	cout << "si::nifi: id=" << id << " depth=" << depth << " maxlevel=" << maxlevel_ << flush;
  uint64 one  = 1;
  uint64 mask = ~(one << (2*depth+1));
  uint64 nodeIndex = (id & mask) + IOFFSET;
  //	cout << " nodeIndex=" << nodeIndex << flush;
  return nodeIndex;
}

uint64 SpatialIndex::nodeIndexFromName(const char* htmIdName) const {
  return nodeIndexFromId(idByName(htmIdName));
}

////////////////////// setMaxlevel /////////////////////////
//void
//SpatialIndex::setMaxlevel(size_t level) {
//	this->maxlevel_ = level;
//}
