
%module pystare

%{
  #define SWIG_FILE_WITH_INIT  /* To import_array() below */
  #include "PySTARE.h"
%}

%include "numpy.i"


%init %{
    import_array();
%}

/* maps ONE double input vector to ONE int64_t output vector of the same length */
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

/* maps ONE int64_t input vector to ONE int output vector of the same length */
/* We use this to create a level/resolution array of the same size as a STARE array. */
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

/* maps ONE int64_t input vector to ONE int64_t output vector of the same length */
/* We use this to create a STARE array ... to_compressed_range */
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

/* maps ONE int64_t input vector one ONE int to ONE int64_t output vector */
/* We use this to create a STARE array ... to_hull_range */
%typemap(in, numinputs=1)
  (int64_t* in_array, int length, int resolution, int64_t* out_array, int len_out, int* result_size)
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
  $3 = (int) array_size(array,0);    
  $4 = (int64_t*) array_data((PyArrayObject*)out);
  $5 = (int*) array_data((PyArrayObject*)(PyArray_SimpleNew(1,1,NPY_INT)));
}

/* maps ONE int64_t input vector to ONE double output vector of the same length */
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

/* maps ONE int64_t input vector to TWO double output vectors of the same length */
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

/* maps ONE int64_t input vector to TWO int64_t output vectors of the same length */
/* We use this to convert STARE intervals to start/terminator arrays to aid comparison */
%typemap(in, numinputs=1)
  (int64_t* in_array, int length, int64_t* out_array1, int64_t* out_array2)
  (PyObject* out1=NULL, PyObject* out2=NULL)
{
  int is_new_object=0;
  npy_intp size[1] = { -1};
  PyArrayObject* array = obj_to_array_contiguous_allow_conversion($input, NPY_INT64, &is_new_object);
  if (!array || !require_dimensions(array, 1)) SWIG_fail;
 
  size[0] = PyArray_DIM(array, 0);  
   
  out1 = PyArray_SimpleNew(1, size, NPY_INT64);
  if (!out1) SWIG_fail;
  out2 = PyArray_SimpleNew(1, size, NPY_INT64);
  if (!out2) SWIG_fail;
   
  $1 = (int64_t*) array_data(array);
  $2 = (int) array_size(array,0);  
  $3 = (int64_t*) array_data((PyArrayObject*)out1);
  $4 = (int64_t*) array_data((PyArrayObject*)out2);
}


/* maps ONE int64_t input array to TWO double and ONE 1D int output array with the same length */
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

/* maps ONE int64_t input array to THREE 1D int output array with the same length */
/* We use this to convert STARE index to triangle vertices & centroid */
%typemap(in, numinputs=1)
  (int64_t* in_array, int length, int64_t* out_array1, int64_t* out_array2, int64_t* out_array3, int64_t* out_array4)
  (PyObject* out1=NULL, PyObject* out2=NULL, PyObject* out3=NULL, PyObject* out4=NULL)
{
  int is_new_object=0;
  npy_intp size[1] = { -1};
  PyArrayObject* array = obj_to_array_contiguous_allow_conversion($input, NPY_INT64, &is_new_object);
  if (!array || !require_dimensions(array, 1)) SWIG_fail;
 
  size[0] = PyArray_DIM(array, 0);  
   
  out1 = PyArray_SimpleNew(1, size, NPY_INT64);
  if (!out1) SWIG_fail;
  out2 = PyArray_SimpleNew(1, size, NPY_INT64);
  if (!out2) SWIG_fail;
  out3 = PyArray_SimpleNew(1, size, NPY_INT64);
  if (!out3) SWIG_fail;
  out4 = PyArray_SimpleNew(1, size, NPY_INT64);
  if (!out4) SWIG_fail;
   
  $1 = (int64_t*) array_data(array);
  $2 = (int) array_size(array,0);  
  $3 = (int64_t*) array_data((PyArrayObject*)out1);
  $4 = (int64_t*) array_data((PyArrayObject*)out2);
  $5 = (int64_t*) array_data((PyArrayObject*)out3);
  $6 = (int64_t*) array_data((PyArrayObject*)out4);
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
    (int64_t* in_array, int length, int64_t* out_array1, int64_t* out_array2)
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

%typemap(argout)
    (int64_t* in_array, int length, int64_t* out_array1, int64_t* out_array2, int64_t* out_array3, int64_t* out_array4)
{
  $result = PyTuple_New(4);
  PyTuple_SetItem($result, 0, (PyObject*)out1$argnum);
  PyTuple_SetItem($result, 1, (PyObject*)out2$argnum);
  PyTuple_SetItem($result, 2, (PyObject*)out3$argnum);
  PyTuple_SetItem($result, 3, (PyObject*)out4$argnum);
}

/* Applying the typemaps */
%apply (double * IN_ARRAY1, int DIM1) {
    (double* lat, int len_lat)        
}

%apply (int64_t * IN_ARRAY1, int DIM1) {
    (int64_t* datetime, int len),
    (int64_t* indices1, int len1),
    (int64_t* indices2, int len2),
    (int64_t* indices, int len)
}

%apply (int64_t * INPLACE_ARRAY1, int DIM1) {
    (int64_t* intersection, int leni),
    (int64_t* range_indices, int len_ri),
    (int64_t* result_size, int len_rs)
}

# %apply (int64_t * ARGOUT_ARRAY1, int DIM1 ) {    
#   (int64_t* range_indices, int len_ri)
# }

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

%apply (int64_t* in_array, int length, int64_t* out_array1, int64_t* out_array2, int64_t* out_array3, int64_t* out_array4) {
    (int64_t* indices, int len, int64_t* vertices0, int64_t* vertices1, int64_t* vertices2, int64_t* centroid)
}

%apply (int64_t* in_array, int length, int64_t* out_array1, int64_t* out_array2) {
	(int64_t* intervals, int len, int64_t* indices_starts, int64_t* indices_terminators )
}

%pythonprepend from_utc(int64_t*, int, int64_t*, int) %{
    import numpy
    datetime = datetime.astype(numpy.int64)
%}

%pythoncode %{
import numpy
def to_compressed_range(indices):
    out_length = len(indices)
    range_indices = numpy.full([out_length],-1,dtype=numpy.int64)
    len_ri = 0
    _to_compressed_range(indices,range_indices)
    endarg = 0
    while (endarg < out_length) and (range_indices[endarg] > 0):
      endarg = endarg + 1
    # endarg = numpy.argmax(range_indices < 0)
    range_indices = range_indices[:endarg]
    return range_indices
    
def to_hull_range(indices,resolution,range_size_limit=1000):
    out_length = range_size_limit
    range_indices = numpy.full([out_length],-1,dtype=numpy.int64)
    result_size = numpy.full([1],-1,dtype=numpy.int64)
    _to_hull_range(indices,resolution,range_indices,result_size)
    range_indices = range_indices[:result_size[0]]
    return range_indices
    
def intersect(indices1, indices2, multiresolution=True):
    out_length = 2*max(len(indices1),len(indices2))
    intersected = numpy.full([out_length],-1,dtype=numpy.int64)
    leni = 0
    if(multiresolution):
      _intersect_multiresolution(indices1, indices2, intersected)
    else:
      _intersect(indices1, indices2, intersected)
    endarg = numpy.argmax(intersected < 0)
    intersected = intersected[:endarg]
    return intersected
%}   
   
%include "PySTARE.h"
