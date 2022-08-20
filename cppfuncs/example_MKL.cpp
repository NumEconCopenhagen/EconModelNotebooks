//////////////////////////
// 1. external includes //
//////////////////////////

// standard C++ libraries
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <omp.h>
#include "mkl.h"

// other
#include "HighResTimer_class.hpp"

///////////////
// 2. macros //
///////////////

#define EXPORT extern "C" __declspec(dllexport)

#define EIGEN_USE_MKL_ALL
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
using namespace Eigen;

/////////////
// 3. main //
/////////////

EXPORT double matmul_BLAS(
    int nrep, int m, int n, int k, 
    double alpha, double beta, double *A, double *B, double *C, 
    int Nthreads_internal, int Nthreads_omp)
{
    
    // C = alpha*A*B + beta*C
    
    // A is m x k
    // B is k x n
    // C is m by n
    
    // note: nrep x nrows x ncols
    
    HighResTimer timer;
    timer.StartTimer();

    mkl_set_num_threads(Nthreads_internal);

    int ncols_A = k;
    int ncols_B = n;
    int ncols_C = n;

    #pragma omp parallel num_threads(Nthreads_omp)
    {    
    
    #pragma omp for 
    for(int i = 0; i < nrep; i++){
                    
        int i_A = i*m*k;
        int i_B = i*k*n;
        int i_C = i*m*n;

        cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans, 
                    m,n,k,alpha,&A[i_A],ncols_A,&B[i_B],ncols_B,beta,&C[i_C],ncols_C);

    } // for
    } // parallel
    
    double time = timer.StopTimer();
    return time;

}

EXPORT double matmul_Eigen(
    int nrep, int m, int n, int k, 
    double alpha, double beta, double *A, double *B, double *C, 
    int Nthreads_internal, int Nthreads_omp)
{
    
    // C = alpha*A*B + beta*C
    
    // A is m x k
    // B is k x n
    // C is m by n
    
    // note: nrep x nrows x ncols
    
    HighResTimer timer;
    timer.StartTimer();
    
    #ifdef EIGEN_USE_MKL_ALL
    mkl_set_num_threads(Nthreads_internal);
    #else
    Eigen::setNbThreads(Nthreads_internal);
    #endif
    
    #pragma omp parallel num_threads(Nthreads_omp)
    {    
    
    #pragma omp for 
    for(int i = 0; i < nrep; i++){
                    
        int i_A = i*m*k;
        int i_B = i*k*n;
        int i_C = i*m*n;

        Map<Matrix<double, Dynamic, Dynamic, RowMajor>> A_mat(&A[i_A],m,k);
        Map<Matrix<double, Dynamic, Dynamic, RowMajor>> B_mat(&B[i_B],k,n);
        Map<Matrix<double, Dynamic, Dynamic, RowMajor>> C_mat(&C[i_C],m,n);
    
        C_mat = alpha*A_mat*B_mat + beta*C_mat;

    } // for
    } // parallel

    double time = timer.StopTimer();
    return time;
        
}

EXPORT double solve_LAPACKE(
    int nrep, int n, int nrhs, 
    double *A, double *b, 
    int Nthreads_internal, int Nthreads_omp)
{
    
    // solve Ax = b
    // A changed to LU
    // b changed to x
    
    // A is nrep x n x n
    // b is nrep x n x nrhs
    
    HighResTimer timer;
    timer.StartTimer();
    
    mkl_set_num_threads(Nthreads_internal);
    
    int nrows = n;
    int ipiv[nrows];
    
    #pragma omp parallel num_threads(Nthreads_omp)
    {

    #pragma omp for  
    for(int i = 0; i < nrep; i++){

        int i_A = i*nrows*nrows;
        int i_b = i*nrows*nrhs;

        LAPACKE_dgesv(LAPACK_ROW_MAJOR,nrows,nrhs,&A[i_A],nrows,ipiv,&b[i_b],nrhs);
    
    } // for
    } // parallel

    double time = timer.StopTimer();
    return time;
    
}  

EXPORT double solve_Eigen(
    int nrep, int n, int nrhs, 
    double *A, double *b, 
    int Nthreads_internal, int Nthreads_omp)
{
    
    // solve Ax = b
    // A changed to LU
    // b changed to x
    
    // A is nrep x n x n
    // b is nrep x n x nrhs

    HighResTimer timer;
    timer.StartTimer();
    
    #ifdef EIGEN_USE_MKL_ALL
    mkl_set_num_threads(Nthreads_internal);
    #else
    Eigen::setNbThreads(Nthreads_internal);
    #endif
    
    int nrows = n;
    int ipiv[nrows];
    
    #pragma omp parallel num_threads(Nthreads_omp)
    {

    #pragma omp for  
    for(int i = 0; i < nrep; i++){

        int i_A = i*nrows*nrows;
        int i_b = i*nrows*nrhs;

        Map<Matrix<double, Dynamic, Dynamic, RowMajor>> A_mat(&A[i_A],n,n);
        Map<Matrix<double, Dynamic, Dynamic, RowMajor>> b_mat(&b[i_b],n,nrhs);

        b_mat = A_mat.lu().solve(b_mat);
        
    } // for
    } // parallel

    double time = timer.StopTimer();
    return time;
    
}  


EXPORT double solve_sparse_Eigen(
    int nrep, int nrow, int ncol, int nnz, 
    int *indptr, int *indices, double *data, double *b, double *x,     
    int Nthreads_internal, int Nthreads_omp)
{

    // solve Ax = b, where A is sparse
    // A is CSR nrow x ncol with nnz non-zero elements
    // A is described by (indptr,indices,data)
    
    HighResTimer timer;
    timer.StartTimer();

    #ifdef EIGEN_USE_MKL_ALL
    mkl_set_num_threads(Nthreads_internal);
    #else
    Eigen::setNbThreads(Nthreads_internal);
    #endif
        
    #pragma omp parallel num_threads(Nthreads_omp)
    {

    #pragma omp for  
    for(int i = 0; i < nrep; i++){
        
        int i_data = i*nnz;
        int i_indices = i_data;
        int i_indptr = i*(ncol+1);
        int i_b = i*nrow;
        int i_x = i_b;

        Map<SparseMatrix<double,RowMajor>> 
            A(nrow,ncol,nnz,&indptr[i_indptr],&indices[i_indices],&data[i_data]);
        Map<Vector<double, Dynamic>> b_vec(&b[i_b],nrow);
        Map<Vector<double, Dynamic>> x_vec(&x[i_x],nrow);

        PardisoLU<SparseMatrix<double,RowMajor>> solver;
        solver.compute(A);
        x_vec = solver.solve(b_vec);

    } // for
    } // parallel  

    double time = timer.StopTimer();
    return time;      

}