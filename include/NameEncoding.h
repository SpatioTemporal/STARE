/*
 * NameEncoding.h
 *
 *  Created on: Jan 23, 2016
 *      Author: mrilee
 */

#ifndef INCLUDE_NAMEENCODING_H_
#define INCLUDE_NAMEENCODING_H_

#include <stddef.h>
#include <string.h>
#include <SpatialGeneral.h>

class NameEncoding {
public:
	NameEncoding() : id(0), name(NULL) {}
	NameEncoding(uint64 id) {setId(id);}
	NameEncoding(const char *name){setName(name);}
	virtual ~NameEncoding();

	virtual char  *nameById(uint64 id)        = 0;
	virtual uint64 idByName(const char *name) = 0;
	virtual uint32 levelById(uint64 id)       = 0;

	uint64 getId() const {return id;}

	void setId(uint64 id) {name = nameById(id);this->id = id;}

	const char* getName() const {return name;}

	void setName(const char* name) {id = idByName(name);this->name = name;}

	bool valid() {
		if(id!=0) {return id == idByName(getName());
		} else {return false;}
	}

private:
	uint64 id;
	const char*  name;
};

#endif /* INCLUDE_NAMEENCODING_H_ */
