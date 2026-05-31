#include "mex.h"
#include "conv57.c"
#include <stdio.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	if (nrhs < 1) { mexErrMsgTxt("Input array required"); }
	if (!mxIsDouble(prhs[0])) { mexErrMsgTxt("Input array must be logical"); }
	double* inp = mxGetPr(prhs[0]);
	size_t inp_len = (size_t)mxGetNumberOfElements(prhs[0]);
	size_t out_len = DEC_LEN(inp_len);
	plhs[0] = mxCreateLogicalMatrix(1, out_len);
	bool* out = mxGetLogicals(plhs[0]);
	// TODO: return as matrix
	trel_soft_t trel = {0};
	dec_soft(inp, inp_len, &trel, out);
}
