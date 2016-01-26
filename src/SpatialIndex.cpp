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

#include <iostream>

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
#include <math.h>
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
SpatialIndex::SpatialIndex(size_t maxlevel, size_t buildlevel) : maxlevel_(maxlevel), 
  buildlevel_( (buildlevel == 0 || buildlevel > maxlevel) ? maxlevel : buildlevel)
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
  float64 v[6][3] = {
    {0.0L,  0.0L,  1.0L}, // 0 k
    {1.0L,  0.0L,  0.0L}, // 1 i
    {0.0L,  1.0L,  0.0L}, // 2 j
   {-1.0L,  0.0L,  0.0L}, // 3 -i
    {0.0L, -1.0L,  0.0L}, // 4 -j
    {0.0L,  0.0L, -1.0L}  // 5 -k
  };

  for(int i = 0; i < 6; i++)
    vertices_[i].set( v[i][0], v[i][1], v[i][2]);

  // create the first 8 nodes - index 1 through 8
  index_ = 1;
  newNode(1,5,2,8,0);  // S0 = numericIdName 8
  newNode(2,5,3,9,0);  // S1 = numericIdName 9
  newNode(3,5,4,10,0); // S2 = numericIdName a
  newNode(4,5,1,11,0); // S3 ...
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
		SpatialVector & v0,
		SpatialVector & v1,
		SpatialVector & v2
		//			 ,bool verbose
) const {

/*

 */

//	cout << "nodeVertex-1000" << endl << flush;
	// TODO Note we need to be very careful about id.

//	if(nodeId64 > nodes_.size()){
//		cout << "nodeVertex error: id: " << nodeId64 << " nodes_.size: " << nodes_.size() << endl << flush;
//	}

	  // TODO Figure out this leaf node indexing and why a depth of 14 is bad.
	  //      Seems related to 32 vs. 64 bits.
	  // buildlevel < maxlevel
	  // get the id of the stored leaf that we are in
	  // and get the vertices of the node we want
//	  uint64 sid = nodeId64 >> ((maxlevel_ - buildlevel_)*2); // Original
	// Note shifting screws up the OFFSET--so does nodeId have an embedded offset or not?
	uint64 sid = (nodeId64 - IOFFSET) >> ((maxlevel_ - buildlevel_)*2);
	uint64 nodeId32 = (uint64)(
			sid );
	// Having an IOFFSET or storedleaves_ here is inconsistent with the table lookup below.
	// But we still need to get beyond the first 9 anyway...
	//			+ IOFFSET); // TODO What's the thinking behind going to the non-leaves?
	//		  - storedleaves_ + IOFFSET); // TODO mlr verify "extra" vertices work.

/* Print node information.
	#define PRINT_(msg,x) {cout << msg << ": " << x << " " << endl << flush;}
	  cout << "nodeVertex" << endl << flush;
	  PRINT_("nodeId64      ",nodeId64);
	  PRINT_("sid          ",sid);
	//  PRINT_("sid - storedleaves_ + IOFFSET",sid - storedleaves_ + IOFFSET);
	  PRINT_("nodeId32     ",nodeId32);
	  PRINT_("maxlevel_    ",maxlevel_);
	  PRINT_("buildlevel_  ",buildlevel_);
	  PRINT_("storedleaves_",storedleaves_);
	  PRINT_("leafCount()  ",leafCount());
	  PRINT_("IOFFSET      ",IOFFSET);
	  PRINT_("nodes_.size  ",nodes_.size());
	#undef PRINT_
*/

 // TODO Vertex by leaf number?
  if(buildlevel_ == maxlevel_) {
	  // TODO Usually buildlevel_ != maxlevel_, so the following code is suspect.
	  // TODO Note that the in the original code we use NodeId64 as an index into nodes_ itself.
	  // TODO If this holds, then we've built and stored all of the nodes.
	  // TODO Need to verify role of IOFFSET here...
//	cout << "nodeVertex-1000-LookingUpVertices" << endl << flush;
    uint64 nodeId32 = (uint64)nodeId64; // Note no shifting or offsetting.
	v0 = vertices_[nodes_[nodeId32].v_[0]];
    v1 = vertices_[nodes_[nodeId32].v_[1]];
    v2 = vertices_[nodes_[nodeId32].v_[2]];
    return;
  }

//  cout << "NOTE: building vertices..." << endl;

  // TODO What stored leaf are we in?  We should cut the extra off of id to get idx.
  v0 = vertices_[nodes_[nodeId32+IOFFSET].v_[0]];
  v1 = vertices_[nodes_[nodeId32+IOFFSET].v_[1]];
  v2 = vertices_[nodes_[nodeId32+IOFFSET].v_[2]];

//  cout << "i,v[]: " << i << " " << ( (nodeId64 - IOFFSET) >> ((maxlevel_ - i)*2) )
//  		<< " [" << v0 << "] [" << v1 << "] [" << v2 << "]" << endl << flush;

  // TODO nodeId64 seems wrong.
  // loop through additional levels,
  // pick the correct triangle accordingly, storing the
  // vertices in v1,v2,v3
  for(uint32 i = buildlevel_ + 1; i <= maxlevel_; i++) {
//	    uint64 j = ( nodeId64 >> ((maxlevel_ - i)*2) ) & 3; // Original
    uint64 j = ( (nodeId64 - IOFFSET) >> ((maxlevel_ - i)*2) ) & 3;
//    uint64 j = ( nodeId64 >> ((maxlevel_ - i)*2) ) -1 ;
//    cout << "  i=" << i << " nv j=ni64 >> (mx-i)*2 " << j << " j&3=" << (j&3) << " " << endl << flush;
//    j = j & 3;

    SpatialVector w0 = v1 + v2; w0.normalize();
    SpatialVector w1 = v0 + v2; w1.normalize();
    SpatialVector w2 = v1 + v0; w2.normalize();

    switch(j) {
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
/* Print info about the dive to higher resolutions.
    cout << "i,v[]: " << i << " " << ( (nodeId64 - IOFFSET) >> ((maxlevel_ - i)*2) )
    		<< " [" << v0 << "] [" << v1 << "] [" << v2 << "]" << endl << flush;
*/
  }
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
 */
float64
SpatialIndex::area(const SpatialVector & v0, 
		   const SpatialVector & v1,
		   const SpatialVector & v2) const {

  float64 a = acos( v0 * v1);
  float64 b = acos( v1 * v2);
  float64 c = acos( v2 * v0);

  float64 s = (a + b + c)/2.0;

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
    *nodes += (size_t)numberOfFaces;
  }
  *vertices = (size_t)numberOfVertices;
  storedleaves_ = numberOfFaces;

  /// Calculate number of leaves.
  i = maxlevel_ - buildlevel_;  /// Recall \p maxlevel_-buildlevel_ is how deep beyond the stored nodes we go.
  while(i-- > 0)
    numberOfFaces *= 4;
  leaves_ = numberOfFaces;
//  dbg cout << "vMax: leaves_, storedleaves_ " << leaves_ << " " << storedleaves_ << endl << flush;
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

// TODO Review IDBYNAME -- uses uint32 works only to 15 levels.  Can we go to 64 easy?

//////////////////IDBYNAME/////////////////////////////////////////////////
/**
 *
 Construct the bit encoded version from the string encoded version of the HTM.

 Note:  This was "Translate ascii leaf name to a uint32," but we have now
 translated this to uint64 and have found no problems so far.

 The following encoding is used:

 The string leaf name has the always the same structure, it begins with
 an N or S, indicating north or south cap and then numbers 0-3 follow
 indicating which child to descend into. So for a depth-5-index we have
 strings like

                 N012023  S000222  N102302  etc

 Each of the numbers correspond to 2 bits of code (00 01 10 11) in the
 uint32. The first two bits are 10 for S and 11 for N. For example

                 S0001122334455     depthSet//Level
                  N 0 1 2 0 2 3     ASCII representation
                 11000110001011  =  12683 (dec)
                  1000110001011  =   4491 (stripped of depthSet bit)

 The leading bits are always 0.

- Legacy 32-bit information:
	-- WARNING: This works only up to 15 levels.
              	 (we probably never need more than 7)
- Converted to 64 bits. Checked to level 27. MLR 2016-01-08


  @param [in] name
  @return HTM index including leading depth bit.
*/
uint64
SpatialIndex::idByName(const char *name) {
// Return the external HTM index. No offsets or other internal indexing.
//	cout << "idByName-1000" << endl << flush;

	uint64 out=0, i;
	uint32 size = 0;

	if(name == 0)              // null pointer-name
		throw SpatialFailure("SpatialIndex:idByName:no name given");
	if(name[0] != 'N' && name[0] != 'S')  // invalid name
		throw SpatialFailure("SpatialIndex:idByName:invalid name",name);

	size = strlen(name);       // determine string length
	/* TODO Note that size contains depth information, but isn't really used beyond
	   sanity checking.
	   */
	// at least size-2 required, don't exceed max
	if(size < 2)
		throw SpatialFailure("SpatialIndex:idByName:invalid name - too short ",name);
	if(size > HTMNAMEMAX)
		throw SpatialFailure("SpatialIndex:idByName:invalid name - too long ",name);

	for(i = size-1; i > 0; i--) {// set bits starting from the end
		if(name[i] > '3' || name[i] < '0') // invalid name
			throw SpatialFailure("SpatialIndex:idByName:invalid name digit ",name);
		out += (uint64(name[i]-'0') << 2*(size - i -1));
	}

	i = 2;                     // set first pair of bits, first bit always set
	if(name[0]=='N') i++;      // for north set second bit too
	out += (i << (2*size - 2) );

	/************************
  // This code may be used later for hashing !
  if(size==2)out -= 8;
  else {
    size -= 2;
    uint32 offset = 0, level4 = 8;
    for(i = size; i > 0; i--) { // calculate 4 ^ (level-1), level = size-2
      offset += level4;
      level4 *= 4;
    }
    out -= level4 - offset;
  }
	 **************************/
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

char *
SpatialIndex::nameById(uint64 id, char * name){

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
  if(!name)
    name = new char[size+1];

  // fill characters starting with the last one
  for(i = 0; i < size-1; i++)
    name[size-i-1] = '0' + char( (id >> i*2) & 3);

  // put in first character
  if( (id >> (size*2-2)) & 1 ) {
    name[0] = 'N';
  } else {
    name[0] = 'S';
  }
  name[size] = 0; // end string

  return name;
}
//////////////////POINTBYID////////////////////////////////////////////////
// Find a vector for the leaf node given by its ID
//

void
SpatialIndex::pointById_mlr1(SpatialVector &vec, uint64 ID) const {
// TODO Check out index->nodeVertex for a possibly correct way to get the right point.
// TODO The way done below may depend on the bitlist and not correctly handle the dynamically created "leaf" index.
	uint32 leafID = this->leafNumberById(ID)+IOFFSET;
//	uint32 leafID = ID-leafCount()+IOFFSET;
//	this->pointById(vec,ID);
	this->pointById(vec,leafID);
}
void
SpatialIndex::pointById(SpatialVector &vec, uint64 ID) const {

	//	uint64 index;
	float64 center_x, center_y, center_z, sum;
	char name[HTMNAMEMAX];
	
	SpatialVector v0, v1, v2; //

	// TODO nodeVertex is expecting something like a leafId
	// TODO nodeVertex might not account for an offset.
	this->nodeVertex(ID, v0, v1, v2);
//	this->nodeVertex(ID, v0, v1, v2, debug);
		
	//??? mlr nameById(ID, name);  // TODO Note: None of these are used!
	// TODO Where is name used?
/*
    I started to go this way until I discovered nameByID...
	Some docs would be nice for this

	switch(name[1]){
	case '0':
		index = name[0] == 'S' ? 1 : 5;
		break;
	case '1':
		index = name[0] == 'S' ? 2 : 6;
		break;
	case '2':
		index = name[0] == 'S' ? 3 : 7;
		break;
	case '3':
		index = name[0] == 'S' ? 4 : 8;
		break;
	}
*/
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
	vec.z_ = center_z; // I don't want it nomralized or radec to be set, 
//	cerr << " - - - - " << endl;
//	vec.show();
//	cerr << "---------- Point by id Retuning" << endl;
}
//////////////////IDBYPOINT////////////////////////////////////////////////
// Find a leaf node where a vector points to
//

uint64
SpatialIndex::idByPoint(SpatialVector & v) const {
    uint64 index;
	uint64 ID;

    // start with the 8 root triangles, find the one which v points to
    for(index=1; index <=8; index++) {
	if( (V(0) ^ V(1)) * v < -gEpsilon) continue;
	if( (V(1) ^ V(2)) * v < -gEpsilon) continue;
	if( (V(2) ^ V(0)) * v < -gEpsilon) continue;
	break;
    }
    // loop through matching child until leaves are reached
    while(ICHILD(0)!=0) {
		uint64 oldindex = index;
		for(size_t i = 0; i < 4; i++) {
			index = nodes_[oldindex].childID_[i];
			if( (V(0) ^ V(1)) * v < -gEpsilon) continue;
			if( (V(1) ^ V(2)) * v < -gEpsilon) continue;
			if( (V(2) ^ V(0)) * v < -gEpsilon) continue;
			break;
		}
    }
    // return if we have reached maxlevel
    if(maxlevel_ == buildlevel_)return N(index).id_;

    // from now on, continue to build name dynamically.
    // until maxlevel_ levels depth, continue to append the
    // correct index, build the index on the fly.
    char name[HTMNAMEMAX];
    nameById(N(index).id_,name);
    size_t len = strlen(name);
    SpatialVector v0 = V(0);
    SpatialVector v1 = V(1);
    SpatialVector v2 = V(2);

    size_t level = maxlevel_ - buildlevel_;
    while(level--) {

		SpatialVector w0 = v1 + v2; w0.normalize();
		SpatialVector w1 = v0 + v2; w1.normalize();
		SpatialVector w2 = v1 + v0; w2.normalize();

		if(isInside(v, v0, w2, w1)) {
			name[len++] = '0';
			v1 = w2; v2 = w1;
			continue;
		} else if(isInside(v, v1, w0, w2)) {
			name[len++] = '1';
			v0 = v1; v1 = w0; v2 = w2;
			continue;
		} else if(isInside(v, v2, w1, w0)) {
			name[len++] = '2';
			v0 = v2; v1 = w1; v2 = w0;
			continue;
		} else if(isInside(v, w0, w1, w2)) {
			name[len++] = '3';
			v0 = w0; v1 = w1; v2 = w2;
			continue;
		}
    }
    name[len] = '\0';
	ID = idByName(name);
	if(0){
		SpatialVector vec;
		pointById(vec, ID);
		//cerr << "pointById: ----------------" << endl;
		//vec.show();
	}    
	return ID;
}

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
	return layers_[depth].firstIndex_;
}

int depthOfName(const char name[]) { return strlen(name)-1; }
int levelOfName(const char name[]) { return strlen(name)-2; } // TODO Move to NameEncoding.
int levelOfDepth(int depth) { return depth-1; } // TODO Remove.

int depthOfId(uint64 htmId) {
	return levelOfId(htmId) + 1;
}
int levelOfId(uint64 htmId) {
	int i;
	uint32 size;
	// determine index of first set bit
	for(i = 0; i < IDSIZE; i+=2) {
		if ( (htmId << i) & IDHIGHBIT ) break;
		if ( (htmId << i) & IDHIGHBIT2 )  // invalid id
			throw SpatialFailure("SpatialIndex:nameById: invalid ID");
	}
	if(htmId == 0)
		throw SpatialFailure("SpatialIndex:nameById: invalid ID");
	size=(IDSIZE-i) >> 1;
	return size-2;
}

uint64 SpatialIndex::nodeIndexFromId(uint64 id) const {
	// This nodeIndex is only valid if depth == maxlevel+1
	// We could fix this to go to the non-leaf parts of the nodes_ array/index.
	int depth = depthOfId(id);
	if (depth != maxlevel_+1) {
		cout << "si:nifi: id=" << id << "maxlevel_=" << maxlevel_ << " depth=" << depth << endl << flush;
		return 0;  // TODO Make this throw an exception?
	}
//	cout << "si::nifi: id=" << id << " depth=" << depth << endl << flush;
	uint64 one  = 1;
	uint64 mask = ~(one << (2*depth+1));
	uint64 nodeIndex = (id & mask) + IOFFSET;
	return nodeIndex;
}

