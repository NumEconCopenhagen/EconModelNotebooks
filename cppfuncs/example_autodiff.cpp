//////////////////////////
// 1. external includes //
//////////////////////////

// standard C++ libraries
#include <stdio.h>

// autodiff include
#include <autodiff/forward/real.hpp>
#include <autodiff/forward/real/eigen.hpp>

// other
#include "par_struct.cpp"


///////////////
// 2. macros //
///////////////

#define EXPORT extern "C" __declspec(dllexport)


/////////////
// 3. main //
/////////////

autodiff::VectorXreal f(const autodiff::VectorXreal& x, par_struct* par)
{

    autodiff::VectorXreal y(par->Ny);

    y[0] = par->alpha*x[0] + exp(x[2]);
    y[1] = par->beta*x[1];

    return y;
}

EXPORT void test(par_struct* par)
{

    // a. input
    autodiff::VectorXreal x(par->Nx);
    for(int i = 0; i < par->Nx; i++){ x[i] = par->x[i];}

    // b. evaluate
    autodiff::VectorXreal y;
    Eigen::MatrixXd Jx = autodiff::jacobian(f, autodiff::wrt(x), autodiff::at(x, par), y); 

    // c. output

    // function value
    for(int i = 0; i < par->Ny; i++){ par->y[i] = (double)y[i];}

    // Jacobian
    int n = 0;
    for(int i = 0; i < par->Ny; i++){ 
        for(int j = 0; j < par->Nx; j++){ 
            par->Jx[n] = (double)Jx(i,j);
            n += 1;
        }
    }

}