#include "mex.h"
#include "conv57.c"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	if (nrhs < 1) { mexErrMsgTxt("Input array required"); }
	if (!mxIsLogical(prhs[0])) { mexErrMsgTxt("Input array must be boolean"); }
	bool* inp = mxGetLogicals(prhs[0]);
	size_t inp_len = (size_t)mxGetNumberOfElements(prhs[0]);
	size_t out_len = ENC_LEN(inp_len);
	plhs[0] = mxCreateDoubleMatrix(out_len, 1, mxCOMPLEX);
	double* out_real = mxGetPr(plhs[0]);
	double* out_imag = mxGetPi(plhs[0]);
	enc(inp, inp_len, out_len, out_real, out_imag);
}
