#include "conv57.c"

int main() {
	bool inp[] = {0,0,0,1,1,0,1,0,0,0};
	size_t inp_len = sizeof(inp)/sizeof(*inp);
	const size_t out_len = ENC_LEN(inp_len);
	double out_real[out_len];
	double out_imag[out_len];
	enc(inp, inp_len, out_len, out_real, out_imag);
	LOG_COMPLEX(out_real, out_imag, out_len);
}
