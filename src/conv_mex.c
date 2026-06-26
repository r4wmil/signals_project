#include "mex.h"
#include <string.h>
#include <stdbool.h>

#define CONV_IMPLEMENTATION
#include "conv.h" // Save your provided C code snippet as conv.h

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs < 5) {
        mexErrMsgTxt("Usage: out = conv_mex(mode, c, G, n, input);");
    }

    // 1. Parse Trellis parameters from arguments
    char mode[16];
    mxGetString(prhs[0], mode, sizeof(mode));
    
    size_t c = (size_t)mxGetScalar(prhs[1]);
    size_t n = (size_t)mxGetScalar(prhs[3]);
    
    size_t num_G = mxGetNumberOfElements(prhs[2]);
    uint32_t *G = malloc(num_G * sizeof(uint32_t));
    double *mxG = mxGetPr(prhs[2]);
    for (size_t i = 0; i < num_G; i++) {
        G[i] = (uint32_t)mxG[i];
    }

    // 2. Generate Trellis on the fly
    conv_trel_t trel = conv_trel_gen(c, G, n);
    size_t ilen = mxGetNumberOfElements(prhs[4]);

    // 3. Process Modes
    if (strcmp(mode, "enc") == 0) {
        bool *inp = (bool*)mxGetLogicals(prhs[4]); // Fixed API function
        size_t olen = conv_get_enc_len(trel, ilen);
        
        plhs[0] = mxCreateLogicalMatrix(1, olen);
        bool *out = (bool*)mxGetLogicals(plhs[0]); // Fixed API function
        
        conv_enc(trel, inp, ilen, olen, out);
    } 
    else if (strcmp(mode, "hard") == 0) {
        bool *inp = (bool*)mxGetLogicals(prhs[4]); // Fixed API function
        size_t olen = ilen / trel.n;
        
        plhs[0] = mxCreateLogicalMatrix(1, olen);
        bool *out = (bool*)mxGetLogicals(plhs[0]); // Fixed API function
        
        conv_hard(trel, inp, ilen, out);
    } 
    else if (strcmp(mode, "soft") == 0) {
        if (!mxIsSingle(prhs[4])) {
            free(G); free(trel.N); free(trel.O);
            mexErrMsgTxt("Soft input must be of type 'single'.");
        }
        float *inp = (float*)mxGetData(prhs[4]);
        size_t olen = ilen / trel.n;
        
        plhs[0] = mxCreateLogicalMatrix(1, olen);
        bool *out = (bool*)mxGetLogicals(plhs[0]); // Fixed API function
        
        conv_soft(trel, inp, ilen, out);
    } 
    else {
        free(G); free(trel.N); free(trel.O);
        mexErrMsgTxt("Invalid mode. Use 'enc', 'hard', or 'soft'.");
    }

    // 4. Free temporary trellis allocations
    free(G);
    free(trel.N);
    free(trel.O);
}
