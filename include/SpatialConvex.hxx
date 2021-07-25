//#     Filename:       SpatialConvex.hxx
//#
//#     H definitions for  SpatialConvex, a more rational naming of RangeConvex.
//#
//#
//#     Author:         Michael Rilee, based on Peter Z. Kunszt, based on A. Szalay's code
//#     
//#     Date:           2018-2021
//#
//#		Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#

extern istream& operator >>( istream&, SpatialConvex &);
extern ostream& operator <<( ostream&, const SpatialConvex &);


inline
SpatialConstraint &
SpatialConvex::operator [](size_t i) {
  return constraints_[i];
}

inline
size_t
SpatialConvex::numConstraints() {
  return constraints_.size();
}
