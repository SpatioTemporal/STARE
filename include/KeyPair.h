/*
 * KeyPair.h
 *
 *  Created on: Jun 23, 2016
 *      Author: mrilee
 */

#include <SkipListElement.h>

#ifndef INCLUDE_KEYPAIR_H_
#define INCLUDE_KEYPAIR_H_

/**
 * struct KeyPair is a lightweight aggregate a single interval [lo,hi] for HtmRange
 */
struct KeyPair {
	Key lo; Key hi; bool set = false;
	KeyPair() { lo = -1; hi = -1; set = false;}
	KeyPair(Key a,Key b) { lo = a; hi = b; set = true; };
};



#endif /* INCLUDE_KEYPAIR_H_ */
