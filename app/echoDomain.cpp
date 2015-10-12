//#     Filename:       echoDomain.cpp
//#
//#     Read a domain file and echo it to screen. Any special domain file
//#     keywords are converted to the standard convex format.
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
#include "fstream.h"
//dcdtmp #include <VarVecDef.h>
#include "SpatialDomain.h"

/*******************************************************
 
  DESCRIPTION
 
  This example code echoes a domain file. It converts special formats
  to the default one.
 
  It can be invoked by
 
  	echoDomain domainfile
 

  where

     domainfile: file containing a domain

  see the description in intersect.cpp for domainfile formats and examples.

*******************************************************/

int
main(int argc, char *argv[]) {

  
  	if(argc <2){
	  cout << "Usage: echoDomain domainfile" << endl;
	  return -1;
	}

	ifstream in(argv[1]);
	if(!in) {
	  cout << "cannot open file! " << endl;
	  return -1;
	}

	SpatialDomain domain;
	in >> domain;
	cout << domain;

	return 0;
}
