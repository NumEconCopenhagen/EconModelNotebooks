//////////////////////////
// 1. external includes //
//////////////////////////

// standard C++ libraries
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <omp.h>
#include "mkl.h"

// other
#include "logs.cpp"

///////////////
// 2. macros //
///////////////

#define PRINT_LEVEL LONG_MAX
#define EXPORT extern "C" __declspec(dllexport)


/////////////
// 3. main //
/////////////

EXPORT void multmat(int m, int n, int k, double alpha, double beta, double *A, double *B, double *C)
{

    logs::create("example_MKL.log");

    logs::write("example_MKL.log",0,"\n This example computes real matrix C=alpha*A*B+beta*C using \n"
            " Intel(R) MKL function dgemm, where A, B, and  C are matrices and \n"
            " alpha and beta are double precision scalars\n\n");

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                m, n, k, alpha, A, k, B, n, beta, C, n);

}