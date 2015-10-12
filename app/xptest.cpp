#include <iostream.h>

#define _INC_IOSTREAM

#include <SpatialGeneral.h>
#include <SpatialIndex.h>
#include <SpatialConstraint.h>
#include <RangeConvex.h>
#include <SpatialDomain.h>
#include <sqlInterface.h>
#include <HtmRange.h>


#include "fstream.h"
#include <time.h>

#define OK_MSG  "OK"
#define MAX_OUTPUT_VECTOR_LENGTH  1000 


///////////////////////////////////////////////////////////////////
// Permute the bytes of an unsigned 64-bit integer so that it is 
// in "string order".   This allows a CAST(string, BigInt) to work in 
// transact SQL.  
void Permute(uint64 source, char *  Target) {
  int	 i;
  for (i=0; i<8 ; i++){			// for each byte
    Target[7-i] = (char)source;	// move next byte to string
    source = source >>8;		// discard that byte
  }
}								// end permute.



int main(int argc, char **argv)
{
  size_t nRanges;
  ValueVector ranges;
  char pzCoordinateSpec[1024];
  char czErrorMsg [1000] = OK_MSG;		// diagnostic message output
  int64		result[MAX_OUTPUT_VECTOR_LENGTH+1]; // the result array that we build locally
  htmSqlInterface htm;
  unsigned int len;							// length (in bytes) of output array

  if (argc < 2){
    cerr << "Need domain specs" << endl;
    return 1;
  }
  strcpy(pzCoordinateSpec, argv[1]);
  nRanges = htm.intersect1(pzCoordinateSpec,ranges);

  if (htm.err()) {						// if error
    strcpy(czErrorMsg,htm.error());		// pass back error message
    if (strlen(czErrorMsg) == 0)		// make up error message if its missing.
      strcpy(czErrorMsg,"HTM code failed to give diagnostic message");
    len = 0;							// result vector is empty
  }									// end error case
  else if (nRanges > MAX_OUTPUT_VECTOR_LENGTH/2) {
    strcpy(czErrorMsg,					// if vector overflowed (we are in BIG trouble).
	   "HTM routines returned more than 500 triangles, overflowed return vector.");
    len = 0;
  } else {					// this is the NO ERROR case
      strcpy(czErrorMsg,OK_MSG);			// construct error message
      len = nRanges*16;					//  each range is 16 bytes (pair of int64)
      for(size_t i = 0; i < nRanges; i++) { // permute them so that they map to strings
	
	cout << ranges[i].lo << " " << ranges[i].hi << endl;
	Permute(ranges[i].lo, (char*) &result[2*i]);
	Permute(ranges[i].hi, (char*) &result[2*i+1]);
      }						 		// end permute
  }	// end no-error case
  cerr << "nRanges = " << nRanges << endl;
  cerr << "RETURNING MESSGAE: ["<<czErrorMsg<<"]" << endl;
  return 0;
}

