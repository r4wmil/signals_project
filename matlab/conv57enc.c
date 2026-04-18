#include "mex.h"
#include <stdio.h>

void enc(bool* d, size_t dl, size_t ol, double* or, double* oi) {
	//for (size_t i = 0; i < dl; i++) {
	//	printf("%d", d[i]);
	//}
	//printf("\n");
	for (size_t i = 0; i < ol; i++) {
		or[i] = +(double)i;
		oi[i] = -(double)i;
	}
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	if (nrhs < 1) { mexErrMsgTxt("Input array required"); }
	if (!mxIsLogical(prhs[0])) { mexErrMsgTxt("Input array must be boolean"); }
	bool* inp = mxGetLogicals(prhs[0]);
	size_t inp_len = (size_t)mxGetNumberOfElements(prhs[0]);
	size_t out_len = (inp_len - 2) * 2;
	plhs[0] = mxCreateDoubleMatrix(out_len, 1, mxCOMPLEX);
	double* out_real = mxGetPr(plhs[0]);
	double* out_imag = mxGetPi(plhs[0]);
	enc(inp, inp_len, out_len, out_real, out_imag);
}
