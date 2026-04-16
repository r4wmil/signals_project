#include "mex.h"
#include <stdio.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs < 1) { mexErrMsgTxt("Input array required"); }
    if (!mxIsLogical(prhs[0])) { mexErrMsgTxt("Input array must be boolean"); }
    bool* inp = mxGetLogicals(prhs[0]);
    mwSize inp_len = mxGetNumberOfElements(prhs[0]);
    for (mwSize i = 0; i < inp_len; i++) {
        printf("%d", inp[i]);
    }
    printf("\n");
}
