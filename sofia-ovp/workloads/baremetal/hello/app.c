

/* $Id: matmult.c,v 1.2 2005/04/04 11:34:58 csg Exp $ */

/* matmult.c */
/* was mm.c! */


/*----------------------------------------------------------------------*
 * To make this program compile under our assumed embedded environment,
 * we had to make several changes:
 * - Declare all functions in ANSI style, not K&R.
 *   this includes adding return types in all cases!
 * - Declare function prototypes
 * - Disable all output
 * - Disable all UNIX-style includes
 *
 * This is a program that was developed from mm.c to matmult.c by
 * Thomas Lundqvist at Chalmers.
 *----------------------------------------------------------------------*/


/*
 * MATRIX MULTIPLICATION BENCHMARK PROGRAM:
 * This program multiplies 2 square matrices resulting in a 3rd
 * matrix. It tests a compiler's speed in handling multidimensional
 * arrays and simple arithmetic.
 */

// #define UPPERLIMIT 256
#include <stdio.h>
#include "FIM.h"


#ifndef OP_PRECISION
#define OP_PRECISION _Float16
#endif

typedef OP_PRECISION matrix [UPPERLIMIT][UPPERLIMIT];
OP_PRECISION a, b, result;
matrix ArrayA, ArrayB, ResultArray;

void main()
{
FIM_Instantiate();
/* ***UPPSALA WCET***:
   no printing please! */
   a=0.5;
   b=0.6;
   result=(a+b)*(a/b);
   printf("Result %f\n",result);

FIM_exit();
}
