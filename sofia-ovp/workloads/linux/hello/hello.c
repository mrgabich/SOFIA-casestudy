
#include "FIM.h"
#include <stdint.h>
#include <stdio.h>

int vec[10];
int i;

int main()
{
FIM_Instantiate();

    printf("startup\n");
    /* start the execution */
    for (i=0;i<10;i++){
        vec[i]=i;
    }
    printf("end\n");

FIM_exit();
return 0;
}
