%module PySTARE1

%{
#define SWIG_FILE_WITH_INIT
	extern double doublefun(double b);
	extern void range(int *rangevec, int n);
%}

%include "numpy.i"

%init %{
import_array();
%}

double doublefun(double b);

/* https://scipy.github.io/old-wiki/pages/Cookbook/SWIG_NumPy_examples */
%apply (int* ARGOUT_ARRAY1, int DIM1) {(int* rangevec, int n)}

void range(int *rangevec, int n);
