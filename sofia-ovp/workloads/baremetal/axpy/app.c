#include <stdlib.h>
#include <math.h>
#include "utils.h" 
#include "FIM.h"

/*************************************************************************/

// Ref version
void axpy_ref(double a, double *dx, double *dy, int n) {
   int i;
   for (i=0; i<n; i++) {
      dy[i] += a*dx[i];
   }
}

void init_vector(double *pv, long n, double value)
{
   for (int i=0; i<n; i++) pv[i]= value;
//   int gvl = __builtin_epi_vsetvl(n, __epi_e64, __epi_m1);
//   __epi_1xi64 v_value   = __builtin_epi_vbroadcast_1xi64(value, gvl);
//   for (int i=0; i<n; ) {
//    gvl = __builtin_epi_vsetvl(n - i, __epi_e64, __epi_m1);
//      __builtin_epi_vstore_1xf64(&dx[i], v_res, gvl);
//     i += gvl;
//   }
}

#ifdef NINPUT
    #define N_ELEMENTS (NINPUT*1024)
#else
    #define N_ELEMENTS (256*1024)
#endif
double dy_ref[N_ELEMENTS];

int main(int argc, char *argv[])
{
    FIM_Instantiate();
    double a=1.0;
    long n = N_ELEMENTS;
    /* Allocate the source and result vectors */
    double *dx     = (double*)malloc(n*sizeof(double));
    double *dy     = (double*)malloc(n*sizeof(double));
    init_vector(dx, n, 1.0);
    init_vector(dy, n, 2.0);
    
    axpy_ref(a, dx, dy, n); 

    capture_ref_result(dy, dy_ref, n);
    test_result(dy, dy_ref, n);
    FIM_exit();
    return 0;
}
