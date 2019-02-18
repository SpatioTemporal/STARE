/*
 * SSTARE.cpp
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

class SSTARE {
public:

	SSTARE() {};
	~SSTARE() {};

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

};

BOOST_PYTHON_MODULE(libSSTARE)
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
		.def("BoundingBoxFromLatLonDegrees", &STARE::BoundingBoxFromLatLonDegrees)
			;

	bp::class_<SSTARE>( "SSTARE", bp::init<>() )
		.def("testD", &SSTARE::testD)
		.def("testF64", &SSTARE::testF64)
		.def("testUI64", &SSTARE::testUI64)
			;


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
