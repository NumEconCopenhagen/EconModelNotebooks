EXPORT  void   fun_nostruct_alt /* weird by purpos */ (double * X , double *Y , 
     int N, double a, double b,/* even weirder */
     int threads, char * txt){

    printf("\nfun_nostruct_alt(...)\n");

    #pragma omp parallel num_threads(threads)
    {

    #pragma omp for      
    for(int i = 0; i < N; i++){
        Y[i] = X[i]*(a+b);
    }

    printf("omp_get_thread_num() = %2d, omp_get_num_procs() = %2d\n",omp_get_thread_num(),omp_get_num_procs());

    } // omp parallel
    
    printf("\n");

}

EXPORT double I_DO_NOTHING()
{
    
    printf("\nI do nothing!\n");
    return 5.0;

}