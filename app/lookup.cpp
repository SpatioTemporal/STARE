//#     Filename:       lookup.cpp
//#
//#     specify a point on the sphere, return its ID/Name to a certain depth
//#
//#
//#     Author:         Peter Z. Kunszt
//#
//#     Date:           October 15, 1999
//#
//#
//#
//# Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#
//# This program is free software; you can redistribute it and/or
//# modify it under the terms of the GNU General Public License
//# as published by the Free Software Foundation; either version 2
//# of the License, or (at your option) any later version.
//#
//# This program is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//#
//# You should have received a copy of the GNU General Public License
//# along with this program; if not, write to the Free Software
//# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//#
//#
#include <stdlib.h>
//dcdtmp #include <VarVecDef.h>
#include "VarStr.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"

/*******************************************************
 
  DESCRIPTION
 
  This example code demonstrates the lookup functionality of
  the SpatialIndex.
 
  It can be invoked by
 
  	lookup level x y z

	or

	lookup level ra dec

  where

     level     : the level depth to build the index (2 - 14)
     x,y,z     : define a point on the unit sphere (can be non-unit vector)
     ra,dec    : "

  it echoes the vector and returns its ID/Name for the given level.


Example 1: level 5, ra,dec = 10,25

%lookup 5 10 25

	(x,y,z) = 0.892539 0.157379 0.422618
	(ra,dec) = 10,25
	ID/Name = 16020 N322110 

Example 2: level 14, x,y,z = -1,2,-23  (output is normed version)

% lookup 14 -1 2 -23

	(x,y,z) = -0.0432742 0.0865485 -0.995307
	(ra,dec) = 116.565,-84.4471
	ID/Name = 2486622255 S110031231200233

*******************************************************/

void
usage(char *name) {

  cout << "usage: " << endl
       << name << " [-hex] [-area] [-corner] [-verbose] [-quiet] [-n n] [-text] depth ( x y z | ra dec )" << endl;
  cout << " [-hex]      : print node id in hexadecimal" << endl
       << " [-area]     : print area of node" << endl
       << " [-corner]   : print node corners" << endl
       << " [-verbose]  : verbose: print all" << endl
       << " [-text]     : submit text command (test interface)" << endl
       << " [-n n]      : do n lookups (speed test)" << endl
       << " [-quiet]    : don't chat, just give back node name" << endl
       << " depth       : level of index to return" << endl
       << " x y z       : cartesian coordinate of point to look up" << endl
       << " ra dec      : J2000 coordinate of point to look up" << endl;
  exit(0);
}

int
main(int argc, char *argv[]) {

//*******************************************************
//
// Initialization
//
//*******************************************************

  bool radec=true;
  bool idlookup=false;
  bool hex=false;		// id in hex
  bool area=false;		// print area of node
  bool corner=false;		// print corners
  bool text = false;		// text flag
  bool quiet = false;		// debug flag
  int args = 1;			// counter
  int arg = 3;			// number of required arguments
  size_t j, depth, n=1;
  char *idname;
  float64 ra, dec, x, y, z;
  VarStr varg;
  htmInterface *htm;
  argc--;

  while(argc > 0) {
    if(strcmp(argv[args],"-hex")==0)
      hex=true;
    else if(strcmp(argv[args],"-area")==0)
      area = true;
    else if(strcmp(argv[args],"-text")==0)
      text = true;
    else if(strcmp(argv[args],"-corner")==0)
      corner = true;
    else if(strcmp(argv[args],"-quiet")==0)
      quiet = true;
    else if(strcmp(argv[args],"-verbose")==0) {
      hex = true;
      corner = true;
      area = true;
    } else if(strcmp(argv[args],"-n")==0) {
      varg = argv[++args];
      if(!htmInterface::isInteger(varg))usage(argv[0]);
      n = atoi(varg.data());
      argc--;
    } else {
      if( *argv[args] == '-' ) usage(argv[0]);
      switch(arg) {
      case 3:
	// build the index to level 'depth'
	if(*argv[args] == 'N' || *argv[args] == 'S') {
	  depth = strlen(argv[args])-2;
	  idlookup = true;
	  idname = argv[args];
	  arg = 0;
	} else {
	  varg = argv[args];
	  if(!htmInterface::isInteger(varg))usage(argv[0]);
	  depth = atoi(argv[args]) ;
	}
	break;
      case 2:
	varg = argv[args];
	if(!htmInterface::isFloat(varg))usage(argv[0]);
	ra = atof(argv[args]);
	break;
      case 1:
	varg = argv[args];
	if(!htmInterface::isFloat(varg))usage(argv[0]);
	dec = atof(argv[args]);
	break;
      case 0:
	x = ra;
	y = dec;
	varg = argv[args];
	if(!htmInterface::isFloat(varg))usage(argv[0]);
	z = atof(argv[args]);
	radec = false;
	break;
      default:
	usage(argv[0]);
      }
      arg--;
    }
    args++;
    argc--;
  }
  if(arg > 0)usage(argv[0]);

  try {

    if(!quiet)
      printf("Depth = %d\n",depth);
    htm = new htmInterface(depth);


// *******************************************************
//
// Normalize Coordinate
//
// ******************************************************

    SpatialVector *v = NULL;
    if(radec)
      v = new SpatialVector(ra,dec);// initialize SpatialVector from ra,dec
    else if (!idlookup){
      v = new SpatialVector(x,y,z); // initialize from x,y,z
      v->normalize();
    }
    if(v) {
      x = v->x(); y = v->y(); z = v->z();
      ra = v->ra(); dec = v->dec();
    }

    if (!quiet && !idlookup) {
      printf("(x,y,z)  = %20.16f, %20.16f, %20.16f\n",x,y,z);
      printf("(ra,dec) = %20.16f, %20.16f\n",ra,dec);
    }

// *******************************************************
//
// Lookup
//
// ******************************************************

    time_t t0 = clock();
    j = n;
    uint64 id;
    if(text) {
      VarStr cmd;
      char str[200];
      if(idlookup) {
	cmd = "NAME ";
	cmd += idname;
      } else if(radec) {
	sprintf(str,"J2000 %d %20.16f %20.16f",depth,ra,dec);
	cmd = str;
      } else {
	sprintf(str,"CARTESIAN %d %20.16f %20.16f %20.16f",depth,x,y,z);
	cmd = str;
      }
      id = htm->lookupIDCmd(cmd.data());
    } else {
      if (idlookup) {
	while(j--)
	  id = htm->lookupID(idname);
      } else if(radec) {
	while(j--) id = htm->lookupID(ra,dec);  // lookup id by ra dec
      } else {
	while(j--) id = htm->lookupID(x,y,z);  // lookup id by x y z
      }
    }

    time_t t1 = clock();
    double nsec = (double)(t1-t0)/(double)CLOCKS_PER_SEC ;
    if(nsec)nsec = (double) n / nsec;
    if(!quiet)
      printf(" Lookup performance: %f /sec\n",nsec);

// *******************************************************
//
// Print result
//
// ******************************************************

    if (!quiet) {
      printf("ID/Name  = ");
      if(hex)
	PRINTID_HEX(id);
      else
	PRINTID(id);
    }
    printf(" %s\n",htm->lookupName(id));

    if(corner) {
      SpatialVector v1,v2,v3;
      htm->index().nodeVertex(id,v1,v2,v3);
      cout.precision(18);
      cout << "Corners :" << endl << v1 << endl
	   << v2 << endl
	   << v3 << endl;
      cout << "Corners (ra/dec):" << endl << v1.ra() << " " << v1.dec() << endl
	   << v2.ra() << " " << v2.dec() << endl
	   << v3.ra() << " " << v3.dec() << endl;
      cout << "Differences : " << endl
	   << v1.x() - v2.x() << " " << v1.y() - v2.y() << " " << v1.z() - v2.z() << endl
	   << v1.x() - v3.x() << " " << v1.y() - v3.y() << " " << v1.z() - v3.z() << endl
	   << v3.x() - v2.x() << " " << v3.y() - v2.y() << " " << v3.z() - v2.z() << endl;
    }
    if(area) {
      cout << "AREA = " << htm->index().area(id) << endl;
    }

  } catch (SpatialException x) {
    printf("%s\n",x.what());
  }

  return 0;
}
