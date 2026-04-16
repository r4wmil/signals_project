#include "mex.h"
#include <stdio.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    double* input_array = mxGetPr(prhs[0]);
    mwSize input_array_len = mxGetNumberOfElements(prhs[0]);
    for (mwSize i = 0; i < input_array_len; i++) {
        printf("%lf\n", input_array[i]);
    }
}