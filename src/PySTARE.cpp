/*
 * PySTARE.cpp
 *
 * Example usage:
 *
 	Python 3.7.2 (default, Jan 16 2019, 19:49:22)
	[GCC 8.2.1 20181215 (Red Hat 8.2.1-6)] on linux
	Type "help", "copyright", "credits" or "license" for more information.
	>>> import numpy as np; from PySTARE import SSTARE; s = SSTARE(); idx=s.ValueFromLatLonDegreesNP(np.array([30,45]),np.array([45,60]),8); lat,lon=s.LatLonDegreesFromValueNP(idx)
	vfldnp: strides: 8
	idx: 4151504989081014792
	ll1: 30, 45
	idx: 4161865161846704136
	ll1: 45, 60
	>>> lat
	array([30.00000057, 45.00000161])
	>>> lon
	array([44.99999922, 59.99999917])
	>>>
 *
 * The round-trip errors are around 0.1 m.
 *
 * Note the error in this round-trip. Some of this could be due to floating point,
 * while some is due to a mismatch between the input location and the nearest
 * triangle center. At level 27 and float64, it's hard to tell these apart.
 *
 *  Created on: Feb 12, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include "STARE.h"

namespace bp = boost::python;
namespace bn = boost::python::numpy;

/*
 * https://stackoverflow.com/questions/1008343/boost-python-and-python-exceptions

void my_runtime_exception_translator(bp::RuntimeException const& ex)
{ PyErr_SetString(PyExc_RuntimeError, ex.toString().c_str()); }
*/

class PySTARE {
public:

	STARE index;

	PySTARE() {};
	~PySTARE() {};

	bn::ndarray testD() {
		std::vector<double> v;
		v.push_back(1); v.push_back(2); v.push_back(3);
		Py_intptr_t shape[1] = { v.size() };
		bn::ndarray result = bn::zeros(1,shape,bn::dtype::get_builtin<double>());
		std::copy(v.begin(), v.end(), reinterpret_cast<double*>(result.get_data()));
		return result;
	}

	bn::ndarray testF64() {
		std::vector<float64> v;
		v.push_back(1); v.push_back(2); v.push_back(3);
		Py_intptr_t shape[1] = { v.size() };
		bn::ndarray result = bn::zeros(1,shape,bn::dtype::get_builtin<float64>());
		std::copy(v.begin(), v.end(), reinterpret_cast<float64*>(result.get_data()));
		return result;
	}

	bn::ndarray testUI64() {
		std::vector<STARE_ArrayIndexSpatialValue> v;
		v.push_back(1); v.push_back(2); v.push_back(3);
		Py_intptr_t shape[1] = { v.size() };
		bn::ndarray result = bn::zeros(1,shape,bn::dtype::get_builtin<STARE_ArrayIndexSpatialValue>());
		std::copy(v.begin(), v.end(), reinterpret_cast<STARE_ArrayIndexSpatialValue*>(result.get_data()));
		return result;
	}

	bn::ndarray testUI64_1(bn::ndarray v, uint64 delta) {
		bn::dtype dtype =  v.get_dtype();
		const Py_intptr_t *shape = {v.get_shape()};
		bn::ndarray result = bn::zeros(1,shape,dtype);
		for(int i=0; i<shape[0]; ++i) {
			result[i] = v[i] + delta;
		}
		return result;
	}

	bn::ndarray testUI64_2(bn::ndarray v, int64 delta) {
		bn::dtype dtype =  v.get_dtype();
		const Py_intptr_t *shape = {v.get_shape()};
		bn::ndarray result = bn::zeros(1,shape,dtype);
		for(int i=0; i<shape[0]; ++i) {
			result[i] = v[i] + delta;
		}
		return result;
	}
// bp::tuple
	bn::ndarray ValueFromLatLonDegreesNP(bn::ndarray lat, bn::ndarray lon, int level = 8) {
		const Py_intptr_t *shape = {lat.get_shape()}; // TODO Fix assumption shape is 1d and stride is 1.
		// TODO Check shape & type of lat & lon and throw exception if bad.
		Py_intptr_t const * strides = lat.get_strides();
		std::cout << "vfldnp: strides: " << strides[0] << std::endl << std::flush;

		bn::ndarray result = bn::zeros(1,shape,bn::dtype::get_builtin<STARE_ArrayIndexSpatialValue>());
		bn::dtype lat_dtype = lat.get_dtype();
		bn::dtype lon_dtype = lon.get_dtype();

		float64 lat_ = -999, lon_ = -999; bool ok = true;

		// TODO Maybe cut down on the logic testing inside the loop
		for(int i=0; ok && (i<shape[0]); ++i) {
			if ( bn::dtype::get_builtin<float64>() == lat_dtype ) {
				lat_ = *reinterpret_cast<float64 const *>( lat.get_data() + i*strides[0] );
			} else if ( bn::dtype::get_builtin<int64>() == lat_dtype ) {
				lat_ = *reinterpret_cast<int64 const *>( lat.get_data() + i*strides[0] );
			} else {
				ok = false;
			}
			if ( bn::dtype::get_builtin<float64>() == lon_dtype ) {
				lon_ = *reinterpret_cast<float64 const *>( lon.get_data() + i*strides[0] );
			} else if ( bn::dtype::get_builtin<int64>() == lat_dtype ) {
				lon_ = *reinterpret_cast<int64 const *>( lon.get_data() + i*strides[0] );
			} else {
				ok = false;
			}
			if( ok ) {
				result[i] = index.ValueFromLatLonDegrees(lat_,lon_,level);
			}
		}
		return result;
	}

	bp::tuple LatLonDegreesFromValueNP(bn::ndarray values) {
		// TODO Add support for ndarray input that is not 1-d.
		const Py_intptr_t *shape = {values.get_shape()};
		Py_intptr_t const * strides = values.get_strides();
		bn::ndarray result_lat = bn::zeros(1,shape,bn::dtype::get_builtin<float64>());
		bn::ndarray result_lon = bn::zeros(1,shape,bn::dtype::get_builtin<float64>());
		for(int i=0; i<shape[0]; ++i) {
			STARE_ArrayIndexSpatialValue idx = *reinterpret_cast<STARE_ArrayIndexSpatialValue*>(values.get_data() + i*strides[0]);
			std::cout << "idx: " << idx << std::endl << std::flush;
			LatLonDegrees64 latlon1 = index.LatLonDegreesFromValue(idx);
			std::cout << "ll1: " << latlon1.lat << ", " << latlon1.lon << std::endl << std::flush;
			result_lat[i] = latlon1.lat;
			result_lon[i] = latlon1.lon;
		}
		return bp::make_tuple(result_lat,result_lon);
	}
};



BOOST_PYTHON_MODULE(PySTARE)
{

	bn::initialize();

	// using namespace boost::python;

	bp::class_<LatLonDegrees64>("LatLonDegrees64",bp::init<float64,float64>() )
			;
	 // class_<LatLonDegrees64ValueVector>( "", init<>() )
			// ;

	bp::class_<STARE>( "STARE", bp::init<>() )
		.def("ValueFromLatLonDegrees", &STARE::ValueFromLatLonDegrees)
		.def("LatLonDegreesFromValue", &STARE::LatLonDegreesFromValue)
		.def("TriangleFromValue",      &STARE::TriangleFromValue)
		.def("AreaFromValue",          &STARE::AreaFromValue)
		.def("CoverBoundingBoxFromLatLonDegrees", &STARE::CoverBoundingBoxFromLatLonDegrees)
		.def("CoverCircleFromLatLonDegrees", &STARE::CoverCircleFromLatLonRadiusDegrees)
			;

	bp::class_<PySTARE>( "SSTARE", bp::init<>() )
		.def("testD",      &PySTARE::testD)
		.def("testF64",    &PySTARE::testF64)
		.def("testUI64",   &PySTARE::testUI64)
		.def("testUI64_1", &PySTARE::testUI64_1)
		.def("testUI64_2", &PySTARE::testUI64_2)
		.def("ValueFromLatLonDegreesNP", &PySTARE::ValueFromLatLonDegreesNP)
		.def("LatLonDegreesFromValueNP", &PySTARE::LatLonDegreesFromValueNP)
			;

	/*
	register_exception_translator<RuntimeException>(my_runtime_exception_translator);
	 */

}
/*
 * https://stackoverflow.com/questions/43107005/exposing-stdvectorstruct-with-boost-python#
 * https://stackoverflow.com/questions/6157409/stdvector-to-boostpythonlist/6158370#6158370
 */

/*
BOOST_PYTHON_MODULE(getting_started2)
{
    // Create the Python type object for our extension class and define __init__ function.
    class_<hello>("hello", init<std::string>())
        .def("greet", &hello::greet)  // Add a regular member function.
        .def("invite", invite)  // Add invite() as a regular function to the module.
    ;

    def("invite", invite); // Even better, invite() can also be made a member of module!!!
}
*/
