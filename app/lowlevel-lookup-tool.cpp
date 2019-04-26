//#     Filename:       lowlevel-lookup-tool.cpp
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
//#     Heavily revised by Michael Rilee, mike@rilee.net, for DERECHOs and STARE, 2015-2019.
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
#include <iomanip>
#include <ios>
//dcdtmp #include <VarVecDef.h>
#include "VarStr.h"
#include "SpatialVector.h"
#include "SpatialInterface.h"
#include "EmbeddedLevelNameEncoding.h"
#include "STARE.h"

/*******************************************************

  DESCRIPTION

  This example code demonstrates the lookup functionality of
  the SpatialIndex.

  It can be invoked by

  	lowlevel-lookup-tool level x y z

	or

	lowlevel-lookup-tool level ra dec

  where

     level     : the level depth to build the index (2 - 14)
     x,y,z     : define a point on the unit sphere (can be non-unit vector)
     ra,dec    : "

  it echoes the vector and returns its ID/Name for the given level.


Example 1: level 5, ra,dec = 10,25

% lowlevel-lookup-tool 5 10 25

	(x,y,z) = 0.892539 0.157379 0.422618
	(ra,dec) = 10,25
	ID/Name = 16020 N322110 

Example 2: level 14, x,y,z = -1,2,-23  (output is normed version)

% lowlevel-lookup-tool 14 -1 2 -23

	(x,y,z) = -0.0432742 0.0865485 -0.995307
	(ra,dec) = 116.565,-84.4471
	ID/Name = 2486622255 S110031231200233

 *******************************************************/

void
usage(char *name) {

	cout << "usage: " << endl
			<< name << " [--hex] [--symbol] [--numeric] [--area] [--corner] [--verbose] [--quiet] [--n n] [--text] [--latlon] depth ( x y z | ra dec | lat lon )" << endl
			<< " e.g. lowlevel-lookup-tool --latlon 5 45.0 45.0" << endl
			<< " e.g. lowlevel-lookup-tool --quiet --hex --STARE --latlon 5 45.0 45.0" << endl
			;
	cout <<    " [--hex]      : print node id in hexadecimal" << endl
			<< " [--dec]      : print node id as a decimal" << endl
			<< " [--symbol]   : print node id as a string symbol" << endl
			<< " [--noBitShift] : do not print bitShifted node id (clobbers -dec if alone)" << endl
			<< " [--area]     : print area of node" << endl
			<< " [--corner]   : print node corners" << endl
			<< " [--verbose]  : verbose: print all" << endl
			<< " [--text]     : submit text command (test interface)" << endl
			<< " [--n n]      : do n lookups (speed test)" << endl
			<< " [--quiet]    : don't chat, just give back node name" << endl
			<< " [--latlon]   : LatLon in degrees of point to look up" << endl
			<< " [--levelEmbedded] : print htm id in embedded level format (full)" << endl
			<< " [--levelEmbeddedIdWithTopBit] : print htm id in embedded level format (level stripped)" << endl
			<< " [--levelEmbeddedIdOnly] : print htm id in embedded level format (level and top bit stripped)" << endl
			<< " [--STARE]   : print STARE spatial id" << endl
			<< " [--HTM]     : print HTM id" << endl
			<< " depth       : level of index to return" << endl
			<< " x y z       : cartesian coordinate of point to look up" << endl
			<< " ra dec      : J2000 coordinate of point to look up" << endl
			<< " lat lon     : latitude longitude of point to look up" << endl
			<< endl
			<< " The command line arguments toggle printing the IDs in order as follows." << endl
			<< " ID 0xID symbol IDEmbedded IDEmbeddedWithTopBit IDEmbeddedIdOnly 0xIDEmbedded 0xIDEmbeddedWithTopBit 0xIDEmbeddedIdOnly STARE" << endl;
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
	bool latlon=false;
	bool idlookup=false;
	bool hex=false;		// id in hex
	bool decimal=false;     // id as decimal
	bool symbol=false;  // id as string symbol
	bool area=false;		// print area of node
	bool corner=false;		// print corners
	bool text = false;		// text flag
	bool quiet = false;		// debug flag
	// bool bitShifted = true; // flag original id
	bool bitShifted = false; // flag original id
	bool levelEmbedded = false;
	bool levelEmbeddedIdWithTopBit = false;
	bool levelEmbeddedIdOnly = false;
	bool STARE_flag = false;
	bool ok = false;

	int args = 1;			// counter
	int arg = 3;			// number of required arguments
	size_t j, depth, n=1;
	char *idname;
	float64 ra, dec, x, y, z;
	VarStr varg;
	htmInterface *htm;
	STARE stareIndex;
	argc--;

	while(argc > 0) {
		if      (strcmp(argv[args],"--hex")==0) hex=true;
		else if (strcmp(argv[args],"--dec")==0) decimal=true;
		else if (strcmp(argv[args],"--symbol")==0) symbol=true;
		else if (strcmp(argv[args],"--noBitShift")==0) {
			bitShifted=false; ok = true;
		}
		else if(strcmp(argv[args],"--latlon")==0) {
			radec=false; latlon=true;
		} else if(strcmp(argv[args],"--levelEmbedded")==0) {
			levelEmbedded = true; ok = true;
		} else if(strcmp(argv[args],"--levelEmbeddedIdWithTopBit")==0) {
			levelEmbeddedIdWithTopBit = true; ok = true;
		} else if(strcmp(argv[args],"--levelEmbeddedIdOnly")==0) {
			levelEmbeddedIdOnly = true; ok = true;
		} else if(strcmp(argv[args],"--STARE")==0) {
			STARE_flag = true; ok = true;
		} else if(strcmp(argv[args],"--HTM")==0) {
			bitShifted = true; ok = true; // cf --noBitShift
		} else if(strcmp(argv[args],"--area")==0)
			area = true;
		else if(strcmp(argv[args],"--text")==0)
			text = true;
		else if(strcmp(argv[args],"--corner")==0)
			corner = true;
		else if(strcmp(argv[args],"--quiet")==0)
			quiet = true;
		else if(strcmp(argv[args],"--verbose")==0) {
			hex = true;
			corner = true;
			area = true;
		} else if(strcmp(argv[args],"--n")==0) {
			varg = argv[++args];
			if(!htmInterface::isInteger(varg))usage(argv[0]);
			n = atoi(varg.data());
			argc--;
		} else {
			//			if( strcmp(argv[args],"--") ) usage(argv[0]); ???
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
	if( arg > 0 || !ok ) {
		usage(argv[0]);
	} else {
		if( !decimal ) { hex = true; }
	}

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
		if(latlon) {
			v = new SpatialVector();
			v->setLatLonDegrees(ra,dec);
		} else if(radec)
			v = new SpatialVector(ra,dec);// initialize SpatialVector from ra,dec
		else if (!idlookup){
			v = new SpatialVector(x,y,z); // initialize from x,y,z
			v->normalize();
		}
		if(v) {
			x = v->x(); y = v->y(); z = v->z();
			if(latlon) {
				v->getLatLonDegrees(ra,dec);
			} else {
				ra = v->ra(); dec = v->dec();
			}
		}

		if (!quiet && !idlookup) {
			//			cout << "radec: " << radec << endl << flush;
			printf("(x,y,z)  = %20.16f, %20.16f, %20.16f\n",x,y,z);
			if(latlon) {
				printf("(lat,lon) = %20.16f, %20.16f\n",ra,dec);
			} else {
				printf("(ra,dec) = %20.16f, %20.16f\n",ra,dec);
			}
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
			} else if (latlon) {
				sprintf(str,"LatLonDegrees %d %20.16f %20.16f",depth,ra,dec);
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

		//		cout << "100" << endl << flush;

		if(symbol||decimal||hex||bitShifted){
			if (!quiet)	printf("ID/Name  = ");
			if(bitShifted) {
				if(!quiet)  cout << " HTM-bitShifted ";
				if(decimal) {
					PRINTID(id); // Original right justified format
				}
				if(hex) {
					cout << " 0x";
					PRINTID_HEX(id);
				}
				if(symbol)  printf(" %s",htm->lookupName(id));
			}

			if(STARE_flag) {
				if(!decimal && !hex) {
					decimal = true;
				}
			}

			if(levelEmbedded||levelEmbeddedIdOnly||levelEmbeddedIdWithTopBit||STARE_flag) {
				EmbeddedLevelNameEncoding *encoding
				= new EmbeddedLevelNameEncoding(htm->lookupName(id));

				ios::fmtflags coutFlags(cout.flags());
				if(decimal) {
					if(levelEmbedded) {
						if(!quiet)  cout << " embeddedLevel-id: ";
						if(decimal) cout << " " << encoding->getId();
					}
					if(levelEmbeddedIdWithTopBit){
						if(!quiet)  cout << " embeddedLevel-idWithTopBit: ";
						if(decimal) cout << " " << encoding->getId_NoEmbeddedLevel();
					}
					if(levelEmbeddedIdOnly){
						if(!quiet)  cout << " embeddedLevel-idOnly: ";
						if(decimal) cout << " " << encoding->bareId_NoShift_NoEmbeddedLevel();
					}
					if(STARE_flag) {
						STARE stareIndex;
						if(!quiet)  cout << " STARE: ";
						if(decimal) cout << " " << stareIndex.ValueFromLatLonDegrees(ra, dec, depth);
					}
				}
				if(hex) {
					cout
					<< std::hex
					<< std::showbase
					<< std::internal
					<< std::setfill('0');

					if(levelEmbedded) {
						if(!quiet)  cout << " embeddedLevel-id: ";
						if(hex)     cout << " " << std::setw(18) << encoding->getId();
					}
					if(levelEmbeddedIdWithTopBit){
						if(!quiet)  cout << " embeddedLevel-idWithTopBit: ";
						if(hex) cout << " " << std::setw(18) << encoding->getId_NoEmbeddedLevel();
					}
					if(levelEmbeddedIdOnly) {
						if(!quiet)  cout << " embeddedLevel-idOnly: ";
						if(hex)     cout << " " << std::setw(18) << encoding->bareId_NoShift_NoEmbeddedLevel();
					}
					if(STARE_flag) {
						if(!quiet) cout << " STARE: ";
						uint64 id1 = stareIndex.ValueFromLatLonDegrees(ra, dec, depth);
						if(hex)    cout << std::setw(18) << id1;
						uint64 htmId1 = stareIndex.htmIDFromValue(id1);
						char idname1[128]; stareIndex.getIndex(depth).nameById(htmId1, idname1);
						if(symbol) cout << " "  << idname1;
					}
					cout << std::dec;
				}
				cout.flags(coutFlags);
			}
			//			if(!(!bitShifted&&decimal))
			cout << endl << flush;
		}

		//		cout << "200" << endl << flush;

		if(corner) {
			SpatialVector v1,v2,v3;

			//			htm->index().nodeVertex(id,v1,v2,v3);  // Bug in the original code.
			htm->index().nodeVertex(htm->index().nodeIndexFromId(id),v1,v2,v3);

			cout.precision(18);
			cout << "Corners :"
					<< endl << v1 << endl
					<< v2 << endl
					<< v3 << endl;
			if(radec) {
				cout << "Corners (ra/dec):"
						<< endl << v1.ra() << " " << v1.dec() << endl
						<< v2.ra() << " " << v2.dec() << endl
						<< v3.ra() << " " << v3.dec() << endl;
			} else if(latlon) {
				float64 lat1, lon1, lat2, lon2, lat3, lon3;
				bool ok = false;
				ok = v1.getLatLonDegrees(lat1,lon1);
				ok = v2.getLatLonDegrees(lat2,lon2);
				ok = v3.getLatLonDegrees(lat3,lon3);

				cout << "Corners (latlon):" << endl
						<< lat1 << " " << lon1 << endl
						<< lat2 << " " << lon2 << endl
						<< lat3 << " " << lon3 << endl;
			}
			cout << "Differences : " << endl
					<< v1.x() - v2.x() << " " << v1.y() - v2.y() << " " << v1.z() - v2.z() << endl
					<< v1.x() - v3.x() << " " << v1.y() - v3.y() << " " << v1.z() - v3.z() << endl
					<< v3.x() - v2.x() << " " << v3.y() - v2.y() << " " << v3.z() - v2.z() << endl;
		}

		//		cout << "300" << endl << flush;

		if(area) {
			//			cout << "AREA = " << htm->index().area(id) << endl; // broken
			cout << "AREA = " << htm->index().area(htm->index().nodeIndexFromId(id)) << endl; // broken
			//			   leafArea = index.area(index.idByLeafNumber(p)); // From htmTest.cpp
			//			cout
			//			<< "nodeIndex vs. idByLeafNumber: " << htm->index().nodeIndexFromId(id)
			//			<< " vs. " << htm->index().idByLeafNumber(id)
			//			<< endl << flush;
		}

		//		cout << "400" << endl << flush;

	} catch (SpatialException x) {
		printf("%s\n",x.what());
	}

	return 0;
}
