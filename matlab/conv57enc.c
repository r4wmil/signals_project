#include <stdio.h>
#include <stdbool.h>

size_t enc_len(size_t l) {
	return (l - 2) * 2;
}

void enc(bool* d, size_t dl, size_t ol, double* or, double* oi) {
	size_t j = 0;
	for (size_t i = 0; i < dl - 2; i++) {
		or[j + 0] = d[i + 0]            ^ d[i + 2];
		or[j + 1] = d[i + 0] ^ d[i + 1] ^ d[i + 2];
		j += 2;
	}
}


#ifdef MATLAB_MEX_FILE
#include "mex.h"
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	if (nrhs < 1) { mexErrMsgTxt("Input array required"); }
	if (!mxIsLogical(prhs[0])) { mexErrMsgTxt("Input array must be boolean"); }
	bool* inp = mxGetLogicals(prhs[0]);
	size_t inp_len = (size_t)mxGetNumberOfElements(prhs[0]);
	size_t out_len = enc_len(inp_len);
	plhs[0] = mxCreateDoubleMatrix(out_len, 1, mxCOMPLEX);
	double* out_real = mxGetPr(plhs[0]);
	double* out_imag = mxGetPi(plhs[0]);
	enc(inp, inp_len, out_len, out_real, out_imag);
}
#else
#error "Unimplemented."
#endif /* MATLAB_MEX_FILE */
