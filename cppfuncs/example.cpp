//////////////////////////
// 1. external includes //
//////////////////////////

// standard C++ libraries
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <omp.h>

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

#include "par_struct.cpp"
#include "example_include.cpp"

EXPORT void fun(par_struct* par){

    logs::create("example.log"); // create empty file
    logs::write("example.log",0,"\nfun(...)\n"); // print if  print_level = 0 <= PRINT_LEVEL

    #pragma omp parallel num_threads(par->threads)
    {

    #pragma omp for   
    for(int i = 0; i < par->N; i++){
        par->Y[i] = par->X[i]*(par->a+par->b);
    }

    logs::write("example.log",0,"omp_get_thread_num() = %2d, omp_get_num_procs() = %2d\n",omp_get_thread_num(),omp_get_num_procs());
    
    } // omp parallel

    logs::write("example.log",0,"string-value: %s\n",par->txt);

    double a = get_double_par_struct(par,par->txt);
    logs::write("example.log",0,"looked up value par->%s = %g\n",par->txt,a);
    
    logs::write("example.log",0,"is \"%s\" in \"%s\": %s\n",par->txt,par->txtlist,(strstr(par->txt,par->txtlist) != NULL) ? "true" : "false");
    
    char* txtlist = (char*) malloc(strlen(par->txtlist)+1);
    strcpy(txtlist,par->txtlist);

    char *txt = strtok(txtlist,"|"); // modifies string internally
    while(txt != NULL){
        int value = get_int_par_struct(par,txt);
        logs::write("example.log",0,"looked up value par->%s = %d\n",txt,value);
        txt = strtok(NULL,"|");
    }
    
    logs::write("example.log",0,"\n");

    free(txtlist);

}

EXPORT void fun_nostruct(double *X, double *Y, int N, double a, double b, int threads, char *txt){
    
    logs::write("example.log",0,"\nfun_nostruct(...)\n");

    #pragma omp parallel num_threads(threads)
    {

    #pragma omp for      
    for(int i = 0; i < N; i++){
        Y[i] = X[i]*(a+b);
    }

    logs::write("example.log",0,"omp_get_thread_num() = %2d, omp_get_num_procs() = %2d\n",omp_get_thread_num(),omp_get_num_procs());

    } // omp parallel
    
    logs::write("example.log",0,"string-value: %s\n",txt);
    if(strcmp(txt,"a") == 0){ logs::write("example.log",0,"test was succesfull\n");        
    } else { logs::write("example.log",0,"test was not succesfull\n"); }

    logs::write("example.log",0,"\n");

}