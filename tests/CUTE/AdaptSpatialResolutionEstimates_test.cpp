
/// Test the adaptSpatialResolutionEstimates() and
/// adaptSpatialResolutionEstimatesInPlace() methods.
///
/// James Gallagher <jgallagher@opendap.org> 4/20/20

#include "Test.h"

#ifndef DIAG
#define DIAG1(expr)
#define DIAGOUT2(out,expr)
#define DIAGOUTDELTA(out,a,b)
#else
#define DIAG1(expr) expr;
#define DIAGOUT2(out,expr) out << expr;
#define DIAGOUTDELTA(out,a,b) {SpatialVector delta_ = a-b; cout << delta_.length() << " ";}
#endif

void AdaptSpatialResolutionEstimates_test() {

	STARE index;

    if(true) {
        double delta = 0.001;
        double lat = 0,lon = 0;
        int lvl = 27;
        EmbeddedLevelNameEncoding lj;
        STARE_ArrayIndexSpatialValues spatialStareIds;

        uint64 source[10] = {
                0x3d7e69d09dbc425b
                ,0x3d7e69d7057d10fb
                ,0x3d7e69d312f1ca1b
                ,0x3d7e69d32945f71b
                ,0x3d7e69da6914455b
                ,0x3d7e69c7c92cde7b
                ,0x3d7e112622b49e5b
                ,0x3d7e115866c6b81b
                ,0x3d7e17a8c067401b
                ,0x3d7e1ab50be8303b
        };

        uint64 result[10] = {
                0x3d7e69d09dbc4250
                ,0x3d7e69d7057d10f0
                ,0x3d7e69d312f1ca0f
                ,0x3d7e69d32945f70e
                ,0x3d7e69da6914454d
                ,0x3d7e69c7c92cde6c
                ,0x3d7e112622b49e4b
                ,0x3d7e115866c6b80a
                ,0x3d7e17a8c0674009
                ,0x3d7e1ab50be83028
        };

        for( int i=0; i < 10; ++i ) {
            spatialStareIds.push_back(index.ValueFromLatLonDegrees(lat,lon,lvl));
            lat += 0; lon += delta; delta += delta;
        }
        STARE_ArrayIndexSpatialValues spatialStareIdsAdapted = index.adaptSpatialResolutionEstimates(spatialStareIds);

        for( int i = 0; i < 10; ++i ) {
            DIAGOUT2(cerr, i << " "
                             << hex << spatialStareIds[i] << " " << spatialStareIdsAdapted[i]
                             << dec << " " << (spatialStareIdsAdapted[i] & lj.levelMaskSciDB) << endl);
            ASSERT_EQUAL(source[i],spatialStareIds[i]);
            ASSERT_EQUAL(result[i],spatialStareIdsAdapted[i]);
        }
    }

    if(true) {
        double delta = 0.001;
        double lat = 0,lon = 0;
        int lvl = 27;
        EmbeddedLevelNameEncoding lj;
        STARE_ArrayIndexSpatialValues spatialStareIds;

        uint64 source[10] = {
                0x3d7e69d09dbc425b
                ,0x3d7e69d7057d10fb
                ,0x3d7e69d312f1ca1b
                ,0x3d7e69d32945f71b
                ,0x3d7e69da6914455b
                ,0x3d7e69c7c92cde7b
                ,0x3d7e112622b49e5b
                ,0x3d7e115866c6b81b
                ,0x3d7e17a8c067401b
                ,0x3d7e1ab50be8303b
        };

        uint64 result[10] = {
                0x3d7e69d09dbc4250
                ,0x3d7e69d7057d10f0
                ,0x3d7e69d312f1ca0f
                ,0x3d7e69d32945f70e
                ,0x3d7e69da6914454d
                ,0x3d7e69c7c92cde6c
                ,0x3d7e112622b49e4b
                ,0x3d7e115866c6b80a
                ,0x3d7e17a8c0674009
                ,0x3d7e1ab50be83028
        };

        for( int i=0; i < 10; ++i ) {
            spatialStareIds.push_back(index.ValueFromLatLonDegrees(lat,lon,lvl));
            lat += 0; lon += delta; delta += delta;
        }

        for( int i = 0; i < 10; ++i ) {
            DIAGOUT2(cerr, i << " " << hex << spatialStareIds[i] << endl);
            ASSERT_EQUAL(source[i],spatialStareIds[i]);
        }

        index.adaptSpatialResolutionEstimatesInPlace(spatialStareIds);

        for( int i = 0; i < 10; ++i ) {
            DIAGOUT2(cerr, i << " "
                             << hex << spatialStareIds[i]
                             << dec << " " << (spatialStareIds[i] & lj.levelMaskSciDB) << endl);
            ASSERT_EQUAL(result[i],spatialStareIds[i]);
        }
    }
}

