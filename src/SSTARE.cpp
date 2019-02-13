/*
 * SSTARE.cpp
 *
 *  Created on: Feb 12, 2019
 *      Author: mrilee
 *
 *  Copyright (C) 2019 Rilee Systems Technologies LLC
 */

#include <boost/python.hpp>

char const* SSTARE() {
	return "SSTARE";
}

BOOST_PYTHON_MODULE(libSSTARE)
{
	using namespace boost::python;
	def("SSTARE",SSTARE);
}


