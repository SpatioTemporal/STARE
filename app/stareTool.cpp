

/* <header> */

//SpatialVector.h
bool SpatialVector::getLatLonDegrees(float64 &lat, float64 &lon);

typedef std::vector<SpatialVector> Vertices;
/// A Triangle struct for convenience & typing. Note vertices must have the appropriate ordering.
// TODO Consider a full-fledged Triangle class to refactor some geometry out of index, etc.
struct Triangle {SpatialVector centroid;  Vertices vertices;};

// STARE.h
Triangle STARE::TriangleFromValue(STARE_ArrayIndexSpatialValue spatialStareId, int resolutionLevel);

/* </header> */


Triangle tr = sIndex.TriangleFromValue(sid,resolution);

std::vector<double> vertices_lats(3),vertices_lons(3);
double centroid_lat,cetroid_lon;

tr.centroid.getLatLonDegrees(centroid_lat,centroid_lon);
for(int i=0; i<3; ++i) {
  tr.vertices[i].getLatLonDegrees(vertices_lats[i],vertices_lons[i]);
}

// Now centroid_lats,centroid_lons and vertices_lats[3],vertices_lons[3]
// have the lat,lon of the various points.

