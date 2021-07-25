/**
   TemporalGeneral.h

   Author: Michael Rilee, Rilee Systems Technologies LLC

   Created: c.2017

 */

#include <vector>

struct Datetime {
	int year, month, day, hour, minute, second, ms;	
};

typedef std::vector<Datetime> DatetimeVector ; 
