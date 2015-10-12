//#     Filename:       timeHTM.cpp
//#
//#     Build the index and time it
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
// dcdtmp #include <VarVecDef.h>
#include "SpatialIndex.h"
#include <time.h>
#include "stdlib.h"
/*******************************************************
 
  DESCRIPTION
 
  This example code just builds the index using exception handling
  and times its initialization.
 
  It can be invoked by
 
  	timeHTM level savelevel
 

  where

     level     : the level depth to build the index (2 - 14)
     savelevel : the level depth to keep index in memory ( <= level)

*******************************************************/

main(int argc,char ** argv) {

  	// initialize
	if(argc < 3) {
	  cout << "Usage: timeHTM level savelevel" << endl;
	  return -1;
	}
	try {
	  // build the index and time it
	  time_t t0 = clock();
	  SpatialIndex index(atoi(argv[1]),atoi(argv[2]));
	  time_t t1 = clock();
	  printf(" Time to build index: %f sec\n",
		 (double) (t1-t0) / (double)CLOCKS_PER_SEC);
	}
	catch (SpatialException x)
	  {
	  }

	return 0;
}
