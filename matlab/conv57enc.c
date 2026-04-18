#include <stdio.h>
#include <stdbool.h>

#define ENC_LEN(_l) (((_l) - 2) * 2)

void enc(bool* d, size_t dl, size_t ol, double* or, double* oi) {
	size_t j = 0;
	for (size_t i = 0; i < dl - 2; i++) {
		or[j + 0] = (double)(d[i + 0]            ^ d[i + 2]) * 2 - 1;
		or[j + 1] = (double)(d[i + 0] ^ d[i + 1] ^ d[i + 2]) * 2 - 1;
		oi[j + 0] = 0;
		oi[j + 1] = 0;
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
int main() {
	bool inp[] = {0,0,0,1,1,0,1,0,0,0};
	size_t inp_len = sizeof(inp)/sizeof(*inp);
	const size_t out_len = ENC_LEN(inp_len);
	double out_real[out_len];
	double out_imag[out_len];
	enc(inp, inp_len, out_len, out_real, out_imag);
	for (size_t i = 0; i < out_len; i++) {
		printf("(%.02lf %.02lf), ", out_real[i], out_imag[i]);
	}
	printf("\n");
}
#endif /* MATLAB_MEX_FILE */
