#include "mex.h"
#include "conv.c"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

bool mx_array_is_int(const mxArray* arr) {
	double val = mxGetScalar(arr);
	return (val == floor(val));
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	if (nrhs < 3) { mexErrMsgTxt("Input required: (constraint_length, generator, feedback)"); }
	if (!mx_array_is_int(prhs[0])) { mexErrMsgTxt("constraint_length must be integer"); }
	uint32_t c_len = (uint32_t)mxGetScalar(prhs[0]);
	printf("%d\n", c_len);
}
