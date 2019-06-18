%module PySTARE1

%{
#define SWIG_FILE_WITH_INIT
/*
	extern double doublefun(double b);
	extern void range(int *rangevec, int n);
	extern double dot(int n, double *a, double *b);
	extern void xdot(int n, double *a, double *b, double *ret);
	*/
#include "STARE.h"
#include "PySTARE1.h"
%}

%include "numpy.i"

%init %{
import_array();
%}

%include "PySTARE1.h"

/* double doublefun(double b); */

/* https://scipy.github.io/old-wiki/pages/Cookbook/SWIG_NumPy_examples */
%apply (int* ARGOUT_ARRAY1, int DIM1) {(int* rangevec, int n)}
void range(int *rangevec, int n);
%clear (int* rangevec, int n);


/* double dot(int n, double *a, double *b); */
%apply (int DIM1, double* IN_ARRAY1) {(int len1, double* vec1),
                                      (int len2, double* vec2)}
%rename (dot) my_dot;
%exception my_dot {
    $action
    if (PyErr_Occurred()) SWIG_fail;
}
%inline %{
double my_dot(int len1, double* vec1, int len2, double* vec2) {
    if (len1 != len2) {
        PyErr_Format(PyExc_ValueError,
                     "Arrays of lengths (%d,%d) given",
                     len1, len2);
        return 0.0;
    }
    return dot(len1, vec1, vec2);
}
%}

/* void xdot(int n, double* a, double* b, double* ret); */
%apply ( int DIM1, double* INPLACE_ARRAY1 ) {(int len3, double* ret)} 
%rename (xdot) my_xdot;
%exception my_xdot {
	$action
	if (PyErr_Occurred()) SWIG_fail;
}
%inline %{
void my_xdot(int len1, double* vec1, int len2, double* vec2, int len3, double* ret) {
	if ( len1 != len2 || len1 != len3 ) {
	           PyErr_Format(PyExc_ValueError,
                     "Arrays of lengths (%d,%d,%d) given",
                     len1, len2, len3);
                     return;
	}
	xdot(len1,vec1,vec2,ret);
}
%}

%clear (int len1, double* vec1);
%clear (int len2, double* vec2);
%clear (int len3, double* ret);

%inline %{

PySTARE1* newSTARE() { return new PySTARE1; }
void deleteSTARE( PySTARE1* p ) { if(p) delete p; p = NULL; }

%}

/* void PySTARE1::ValueFromJDTAI( int len, int64_t* indices, double* JDTAI, int resolution_days ) ; */
%apply ( int DIM1, int64_t* INPLACE_ARRAY1 ) {(int len1, int64_t* indices)}
%apply ( int DIM1, double* IN_ARRAY1 ) {(int len2, double* JDTAI)}
%rename (ValueFromJDTAINP) my_ValueFromJDTAINP;
%exception my_ValueFromJDTAINP {
	$action
	if (PyErr_Occurred()) SWIG_fail;
}

%inline %{
void ValueFromJDTAINP( PySTARE1* p, int len1, int64_t* indices, int len2, double* JDTAI, int resolution_days ) {
	if ( len1 != len2 ) {
		PyErr_Format(PyExc_ValueError,
         "Arrays of lengths (%d,%d) given",
         len1, len2);
        return;
	}
	p->ValueFromJDTAINP(len1, indices, JDTAI, resolution_days );
}
%}
%clear (int len1, int64_t* indices);
%clear (int len2, double* JDTAI);

/* void PySTARE1::JDTAIFromValueNP( int len, double* JDTAI, int64_t* indices ) */
%apply ( int DIM1, double* IN_ARRAY1 ) {(int len1, double* JDTAI)}
%apply ( int DIM1, int64_t* INPLACE_ARRAY1 ) {(int len2, int64_t* indices)}

%rename (JDTAIFromValueNP) my_JDTAIFromValueNP;
%exception my_JDTAIFromValueNP {
	$action
	if (PyErr_Occurred()) SWIG_fail;
}

%inline %{
void my_JDTAIFromValueNP( PySTARE1* p, int len1, double* JDTAI, int len2, int64_t* indices) {
	if ( len1 != len2 ) {
		PyErr_Format(PyExc_ValueError,
         "Arrays of lengths (%d,%d) given",
         len1, len2);
        return;
	}
	p->JDTAIFromValueNP(len1,  JDTAI, indices );
}
%}
%clear (int len1, double* JDTAI);
%clear (int len2, int64_t* indices);


/* void ValueFromLatLonDegreesLevelNP( int len, int64_t* indices, double* lat, double* lon, int resolutionLevel ); */
%apply ( int DIM1, int64_t* INPLACE_ARRAY1 ) {(int len1, int64_t* indices)}
%apply ( int DIM1, double* IN_ARRAY1 ) {(int len2, double* lat),(int len3, double* lon)}

%rename (ValueFromLatLonDegreesLevelNP) my_ValueFromLatLonDegreesLevelNP;
%exception  my_ValueFromLatLonDegreesLevelNP{
	$action
	if (PyErr_Occurred()) SWIG_fail;
}

%inline %{
void my_ValueFromLatLonDegreesLevelNP( PySTARE1* p, int len1, int64_t *indices, int len2, double* lat, int len3, double* lon, int resolutionLevel) {

	if ( len1 != len2 || len1 != len3 ) {
		PyErr_Format(PyExc_ValueError,
         "Arrays of lengths (%d,%d,%d) given",
         len1, len2, len3);
        return;
	}
	p->ValueFromLatLonDegreesLevelNP(len1, indices, lat, lon, resolutionLevel);
}
%}

%clear (int len1, int64_t* indices);
%clear (int len2, double* lat);
%clear (int len3, double* lon);



/* void LatLonDegreesFromValueNP( int len, double* lat, double* lon, int64_t* indices ); */
%apply ( int DIM1, double* INPLACE_ARRAY1 ) {(int len1, double* lat),(int len2, double* lon)}
%apply ( int DIM1, int64_t* IN_ARRAY1 ) {(int len3, int64_t* indices)}

%rename (LatLonDegreesFromValueNP) my_LatLonDegreesFromValueNP;
%exception my_LatLonDegreesFromValueNP {
	$action
	if (PyErr_Occurred()) SWIG_fail;
}

%inline %{
void my_LatLonDegreesFromValueNP( PySTARE1* p, int len1, double* lat, int len2, double* lon, int len3, int64_t* indices ) {
	if ( len1 != len2 || len1 != len3 ) {
		PyErr_Format(PyExc_ValueError,
         "Arrays of lengths (%d,%d,%d) given",
         len1, len2, len3);
        return;
	}
	p->LatLonDegreesFromValueNP( len1, lat, lon, indices );
}
%}
%clear (int len1, double* lat);
%clear (int len2, double* lon);
%clear (int len3, int64_t* indices);

