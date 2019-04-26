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
	virtual uint32 levelById(uint64 id) const = 0;
	uint32 levelByName(const char *name) { return strlen(name)-2; }

	virtual uint64 bareId() const = 0; ///< Return id without top (level) bit set.

	virtual int getLocalTriangleNumber() const = 0; /// Which of the leaf-siblings is this triangle?

	uint64 getId() const {return id;}

	/// Sets this->id immediately.
	void setId(uint64 id) {name = nameById(id);this->id = id;}

	virtual uint64 maskOffLevel(uint64 id) = 0;
	virtual uint64 maskOffLevel() = 0;

	const char* getName() const {return name;}

	void setName(const char* name) {id = idByName(name);this->name = name;}

	virtual const char* getEncodingType() const = 0;

	const uint32 getLevel() const { return levelById(id); }

	bool valid() {
		if(id!=0) {return id == idByName(getName());
		} else {return false;}
	}

protected:
	/// The fundamental id that is encoded.
	uint64 id;
	/// The symbolic name associated with the id, e.g. N01230123.
	const char*  name;

};

#endif /* INCLUDE_NAMEENCODING_H_ */
