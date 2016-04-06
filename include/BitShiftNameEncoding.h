/*
 * BitShiftNameEncoding.h
 *
 *  Created on: Jan 23, 2016
 *      Author: mrilee
 */

#ifndef INCLUDE_BITSHIFTNAMEENCODING_H_
#define INCLUDE_BITSHIFTNAMEENCODING_H_

#include <SpatialException.h>
#include "NameEncoding.h"

/// Right justified.
class BitShiftNameEncoding: public NameEncoding {
public:
	BitShiftNameEncoding();
	BitShiftNameEncoding(uint64 id) {setId(id);}
	BitShiftNameEncoding(const char *name){setName(name);}
	virtual ~BitShiftNameEncoding();

	char  *nameById(uint64 id);
	uint64 idByName(const char *name);
	uint32 levelById(uint64 id) const;

	uint64 bareId() const; ///< Return id without top (depth) bit set.

	const char* getEncodingType() const {return "BitShiftedNameEncoding";}

};

#endif /* INCLUDE_BITSHIFTNAMEENCODING_H_ */
