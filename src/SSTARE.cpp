/*
 * SSTARE.cpp
 *
 *  Created on: Feb 12, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include <boost/python.hpp>
#include "STARE.h"

char const* SSTARE() {
	return "SSTARE";
}

BOOST_PYTHON_MODULE(libSSTARE)
{
	using namespace boost::python;

	class_<LatLonDegrees64>("LatLonDegrees64",init<float64,float64>() )
			;
	 // class_<LatLonDegrees64ValueVector>( "", init<>() )
			// ;

	class_<STARE>( "STARE", init<>() )
		.def("ValueFromLatLonDegrees", &STARE::ValueFromLatLonDegrees)
		.def("LatLonDegreesFromValue", &STARE::LatLonDegreesFromValue)
		.def("TriangleFromValue",      &STARE::TriangleFromValue)
		.def("AreaFromValue",          &STARE::AreaFromValue)
		.def("BoundingBoxFromLatLonDegrees", &STARE::BoundingBoxFromLatLonDegrees)
			;

	def("SSTARE",SSTARE);

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
