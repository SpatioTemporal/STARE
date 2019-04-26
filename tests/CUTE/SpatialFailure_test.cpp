/*
 * SpatialFailure_test.cpp
 *
 *  Created on: Jan 29, 2019
 *      Author: mrilee
 */

#include "Test.h"

void SpatialFailure_test() {

	// SpatialFailure( const char *context, const char *operation
	//		, const char *resource, const char *because );

	/*
	cout << "what: " << SpatialFailure(NULL,      "operation", "resource", "because").what();
	cout << "what: " << SpatialFailure("context", NULL,        "resource", "because").what();
	cout << "what: " << SpatialFailure("context", "operation", NULL,       "because").what();
	cout << "what: " << SpatialFailure("context", "operation", "resource", NULL     ).what();
	*/

	/*
	cout << "what: '" << SpatialFailure("context", "operation", "resource", "because").what() << "'" << endl;
	cout << "what: '" << SpatialFailure("context", "operation", "resource", NULL     ).what() << "'" << endl;
	cout << "what: '" << SpatialFailure("context", "operation", NULL      , NULL     ).what() << "'" << endl;
	cout << "what: '" << SpatialFailure("context", NULL       , NULL      , NULL     ).what() << "'" << endl;
	cout << "what: '" << SpatialFailure(NULL,      NULL       , NULL      , NULL     ).what() << "'" << endl;
	*/

	// cout << "what: '" << SpatialInterfaceError("context", "argument", "because").what() << "'" << endl;
	ASSERT_EQUAL("context: argument argument is invalid because because",SpatialInterfaceError("context", "argument", "because").what());

	ASSERT_EQUAL("context: operation failed on resource because because", SpatialFailure("context", "operation", "resource", "because").what());
	ASSERT_EQUAL("context: operation failed on resource",                 SpatialFailure("context", "operation", "resource", NULL     ).what());
	ASSERT_EQUAL("context: operation failed ",                            SpatialFailure("context", "operation", NULL      , NULL     ).what());
	ASSERT_EQUAL("context:  failed operation",                            SpatialFailure("context", NULL       , NULL      , NULL     ).what());
	ASSERT_EQUAL("STARE Library:  failed operation",                      SpatialFailure(NULL,      NULL       , NULL      , NULL     ).what());

}

