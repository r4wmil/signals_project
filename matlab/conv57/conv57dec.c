#include "mex.h"
#include <stdio.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs < 1) { mexErrMsgTxt("Input array required"); }
    if (!mxIsDouble(prhs[0])) { mexErrMsgTxt("Input array must be double or complex"); }
    double* inp_real = mxGetPr(prhs[0]);
    double* inp_imag = mxGetPi(prhs[0]);
    mwSize inp_len = mxGetNumberOfElements(prhs[0]);
    for (mwSize i = 0; i < inp_len; i++) {
        printf("%lf", inp_real[i]);
        if (inp_imag) { printf("+i%lf ", inp_imag[i]); }
    }
    printf("\n");
}
