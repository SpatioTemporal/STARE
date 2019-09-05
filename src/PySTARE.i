
%module pystare

%{
  #define SWIG_FILE_WITH_INIT  /* To import_array() below */
  #include "PySTARE.h"
%}

%include "numpy.i"


%init %{
    import_array();
%}


/* maps 1-D double input array to ONE 1-D integer output array with the same length */
/* We use this to create a STARE array of the same size as a longitude array. 
/* The latitude array is passed separately. We have to do it separately since we can only map ONE input at a time */
/* Consequently, we are not verifying that lat and lon are the same length within the typemap */
%typemap(in, numinputs=1)
  (double* in_array, int length, int64_t* out_array)
   (PyObject* out=NULL)
{
  int is_new_object=0;
  npy_intp size[1] = { -1};
  PyArrayObject* array = obj_to_array_contiguous_allow_conversion($input, NPY_DOUBLE, &is_new_object);
  if (!array || !require_dimensions(array, 1)) SWIG_fail;
  size[0] = PyArray_DIM(array, 0);    
  out = PyArray_SimpleNew(1, size, NPY_INT64);
  if (!out) SWIG_fail;  
  $1 = (double*) array_data(array);
  $2 = (int) array_size(array,0);    
  $3 = (int64_t*) array_data((PyArrayObject*)out);
}

/* maps 1-D int64_t input array to ONE 1-D integer output array with the same length */
/* We use this to create a level array of the same size as a STARE array. */
%typemap(in, numinputs=1)
  (int64_t* in_array, int length, int* out_array)
   (PyObject* out=NULL)
{
  int is_new_object=0;
  npy_intp size[1] = { -1};
  PyArrayObject* array = obj_to_array_contiguous_allow_conversion($input, NPY_INT64, &is_new_object);
  if (!array || !require_dimensions(array, 1)) SWIG_fail;
  size[0] = PyArray_DIM(array, 0);    
  out = PyArray_SimpleNew(1, size, NPY_INT);
  if (!out) SWIG_fail;  
  $1 = (int64_t*) array_data(array);
  $2 = (int) array_size(array,0);    
  $3 = (int*) array_data((PyArrayObject*)out);
}

/* maps 1-D int64_t input array to ONE 1-D int64_t output array with the same length */
/* We use this to create a STARE array of the same size as a numpy64 array. */
%typemap(in, numinputs=1)
  (int64_t* in_array, int length, int64_t* out_array)
   (PyObject* out=NULL)
{
  int is_new_object=0;
  npy_intp size[1] = { -1};
  PyArrayObject* array = obj_to_array_contiguous_allow_conversion($input, NPY_INT64, &is_new_object);
  if (!array || !require_dimensions(array, 1)) SWIG_fail;
  size[0] = PyArray_DIM(array, 0);    
  out = PyArray_SimpleNew(1, size, NPY_INT64);
  if (!out) SWIG_fail;  
  $1 = (int64_t*) array_data(array);
  $2 = (int) array_size(array,0);    
  $3 = (int64_t*) array_data((PyArrayObject*)out);
}

/* maps 1-D int64_t input array to ONE 1-D double output array with the same length */
/* We use this to create an area array of the same size as a STARE array. */
%typemap(in, numinputs=1)
  (int64_t* in_array, int length, double* out_array)
   (PyObject* out=NULL)
{
  int is_new_object=0;
  npy_intp size[1] = { -1};
  PyArrayObject* array = obj_to_array_contiguous_allow_conversion($input, NPY_INT64, &is_new_object);
  if (!array || !require_dimensions(array, 1)) SWIG_fail;
  size[0] = PyArray_DIM(array, 0);    
  out = PyArray_SimpleNew(1, size, NPY_DOUBLE);
  if (!out) SWIG_fail;  
  $1 = (int64_t*) array_data(array);
  $2 = (int) array_size(array,0);    
  $3 = (double*) array_data((PyArrayObject*)out);
}

/* maps 1-D integer input array to TWO 1-D double output array with the same length */
/* We use this to convert STARE index to lat+lon */
%typemap(in, numinputs=1)
  (int64_t* in_array, int length, double* out_array1, double* out_array2)
  (PyObject* out1=NULL, PyObject* out2=NULL)
{
  int is_new_object=0;
  npy_intp size[1] = { -1};
  PyArrayObject* array = obj_to_array_contiguous_allow_conversion($input, NPY_INT64, &is_new_object);
  if (!array || !require_dimensions(array, 1)) SWIG_fail;
 
  size[0] = PyArray_DIM(array, 0);  
   
  out1 = PyArray_SimpleNew(1, size, NPY_DOUBLE);
  if (!out1) SWIG_fail;
  out2 = PyArray_SimpleNew(1, size, NPY_DOUBLE);
  if (!out2) SWIG_fail;
   
  $1 = (int64_t*) array_data(array);
  $2 = (int) array_size(array,0);  
  $3 = (double*) array_data((PyArrayObject*)out1);
  $4 = (double*) array_data((PyArrayObject*)out2);
}

/* maps 1-D double input array to TWO 1-D double and ONE 1D int output array with the same length */
/* We use this to convert STARE index to lat+lon+level */
%typemap(in, numinputs=1)
  (int64_t* in_array, int length, double* out_array1, double* out_array2, int* out_array3)
  (PyObject* out1=NULL, PyObject* out2=NULL, PyObject* out3=NULL)
{
  int is_new_object=0;
  npy_intp size[1] = { -1};
  PyArrayObject* array = obj_to_array_contiguous_allow_conversion($input, NPY_INT64, &is_new_object);
  if (!array || !require_dimensions(array, 1)) SWIG_fail;
 
  size[0] = PyArray_DIM(array, 0);  
   
  out1 = PyArray_SimpleNew(1, size, NPY_DOUBLE);
  if (!out1) SWIG_fail;
  out2 = PyArray_SimpleNew(1, size, NPY_DOUBLE);
  if (!out2) SWIG_fail;
  out3 = PyArray_SimpleNew(1, size, NPY_INT);
  if (!out3) SWIG_fail;
   
  $1 = (int64_t*) array_data(array);
  $2 = (int) array_size(array,0);  
  $3 = (double*) array_data((PyArrayObject*)out1);
  $4 = (double*) array_data((PyArrayObject*)out2);
  $5 = (int*) array_data((PyArrayObject*)out3);
}


/****************/
/* OUT typemaps */
/****************/

%typemap(argout)
  (double* in_array, int length, int64_t* out_array)
{
  $result = (PyObject*)out$argnum;
}

%typemap(argout)
  (int64_t* in_array, int length, int* out_array)
{
  $result = (PyObject*)out$argnum;
}

%typemap(argout)
  (int64_t* in_array, int length, int64_t* out_array)
{
  $result = (PyObject*)out$argnum;
}

%typemap(argout)
  (int64_t* in_array, int length, double* out_array)
{
  $result = (PyObject*)out$argnum;
}

%typemap(argout)
    (int64_t* in_array, int length, double* out_array1, double* out_array2)
{
  $result = PyTuple_New(2);
  PyTuple_SetItem($result, 0, (PyObject*)out1$argnum);
  PyTuple_SetItem($result, 1, (PyObject*)out2$argnum);
}

%typemap(argout)
    (int64_t* in_array, int length, double* out_array1, double* out_array2, int* out_array3)
{
  $result = PyTuple_New(3);
  PyTuple_SetItem($result, 0, (PyObject*)out1$argnum);
  PyTuple_SetItem($result, 1, (PyObject*)out2$argnum);
  PyTuple_SetItem($result, 2, (PyObject*)out3$argnum);
}



/* Applying the typemaps */
%apply (double * IN_ARRAY1, int DIM1) {
    (double* lat, int len_lat)              
}

%apply (int64_t * IN_ARRAY1, int DIM1) {
    (int64_t* datetime, int len)            
}

%apply (double* in_array, int length, int64_t* out_array) {
    (double* lon, int len_lon, int64_t* indices)
}

%apply (int64_t* in_array, int length, int* out_array) {
    (int64_t* indices, int len,  int* levels)
}

%apply (int64_t* in_array, int length, int64_t* out_array) {
    (int64_t* datetime, int len, int64_t* indices)
}

%apply (int64_t* in_array, int length, double* out_array) {
    (int64_t* indices, int len,  double* areas)
}

%apply (int64_t* in_array, int length, double* out_array1, double* out_array2) {
    (int64_t* indices, int len, double* lat, double* lon)
}

%apply (int64_t* in_array, int length, double* out_array1, double* out_array2, int* out_array3) {
    (int64_t* indices, int len, double* lat, double* lon, int* levels)
}

// 

%pythonprepend from_utc(int64_t*, int, int64_t*, int) %{
    import numpy
    datetime = datetime.astype(numpy.int64)
%}


%include "PySTARE.h"
