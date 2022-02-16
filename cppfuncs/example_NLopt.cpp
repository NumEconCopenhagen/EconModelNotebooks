//////////////////////////
// 1. external includes //
//////////////////////////

// standard C++ libraries
#include <cstdio>
#include <cmath>
#include <windows.h>

// other
#include "nlopt-2.4.2-dll64\nlopt.h"
#include "HighResTimer_class.hpp"
#include "logs.cpp"


///////////////
// 2. macros //
///////////////

#define PRINT_LEVEL LONG_MAX
#define EXPORT extern "C" __declspec(dllexport)


///////////////
// 3. struct //
///////////////

typedef struct {
    
    int evals;
    double time;
    double m, alpha, beta;

} solver_struct;


///////////////////////////
// 4. objective function //
///////////////////////////

double objfunc(unsigned n, const double *x, double *grad, void *solver_data_in)
{

        HighResTimer timer;
        timer.StartTimer();

    int rep = 5000000;

    solver_struct *solver_data = (solver_struct *) solver_data_in;
    double alpha = solver_data->alpha;
    double beta  = solver_data->beta;

    if (grad) {
        grad[0] = 2.0*(x[0]-alpha);
        grad[1] = 2.0*(x[0]-beta);
    }

    double obj = 0.0;
    for(int i = 0; i < rep; i++){
        obj += -(pow((x[0]-alpha),2)+pow((x[1]-beta),2));
    }
    obj /= (double)rep;

        solver_data->evals++;
        solver_data->time += timer.StopTimer();

    return -obj;

}


///////////////////////////////
// 5. inequality constraints //
///////////////////////////////

double ineq_constraint(unsigned n, const double *x, double *grad, void *solver_data_in)
{

        HighResTimer timer;
        timer.StartTimer();

    solver_struct *solver_data = (solver_struct *) solver_data_in;

    if (grad) {
        grad[0] = 1.0;
        grad[1] = 1.0;
    }

    double diff = (x[0] + x[1]) - solver_data->m;

        solver_data->time += timer.StopTimer();

    return diff; // positive -> violated
                 // 
 }


/////////////
// 6. main //
/////////////

EXPORT void optimize()
{

    double lb[2], ub[2], x[2];
    logs::create("example_NLopt.log");
    
    // 1. allocate
    solver_struct* solver_data = new solver_struct;

    // 2. settings
    int dim = 2;
    solver_data->m     = 0.6;
    solver_data->alpha = 0.35;
    solver_data->beta  = 0.5;

    solver_data->evals = 0;
    solver_data->time  = 0.0;

    // 3. create optimization object
    auto opt = nlopt_create(NLOPT_LD_MMA, dim); // NLOPT_GN_ORIG_DIRECT

        // settings
        nlopt_set_min_objective(opt, objfunc, solver_data);
        nlopt_set_xtol_rel(opt, 1e-4);
        nlopt_set_maxeval(opt, 500);
        
        // bounds
        lb[0] = 0;
        lb[1] = 0;
        nlopt_set_lower_bounds(opt, lb);

        ub[0] = solver_data->m;
        ub[1] = solver_data->m;
        nlopt_set_upper_bounds(opt, ub);

        // constraints
        nlopt_add_inequality_constraint(opt, ineq_constraint, solver_data, 1e-8);

        // guess
        x[0] = solver_data->m/2.0;
        x[1] = solver_data->m/2.0;

     // 3. run optimizers
    logs::write("example_NLopt.log",0,"");
    double minf;

        HighResTimer timer;
        timer.StartTimer();

    int flag;

    for(int i = 0; i < 10; i++){
        flag = nlopt_optimize(opt, x, &minf);
    }

        double time = timer.StopTimer();

    if(flag < 0) {
        logs::write("example_NLopt.log",0,"nlopt failed!\n");
    }
    else {
        logs::write("example_NLopt.log",0,"found minimum at f(%g,%g) = %0.10g\n", x[0], x[1], minf);
        logs::write("example_NLopt.log",0,"time: %5.2f, inside %5.2f, evals = %d\n",time, solver_data->time, solver_data->evals);
    }

    // 4. destoy optimizers
    nlopt_destroy(opt);

} // optimize