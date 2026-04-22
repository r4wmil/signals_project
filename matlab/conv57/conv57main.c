#include "conv57.c"

int main() {
	bool inp[] = {0,0,0,1,1,0,1,0,0,0};
	bool out[] = {0,0,0,0,0,0,0,0,0,0};
	size_t inp_len = sizeof(inp)/sizeof(*inp);
	size_t out_len = sizeof(out)/sizeof(*out);
	LOG_BINARY(inp, inp_len);
	const size_t enc_len = ENC_LEN(inp_len);
	bool enc[enc_len];
	enc_hard(inp, inp_len, enc);
	LOG_BINARY(enc, enc_len);
	//printf("ok=%d\n", dec_no_err(enc_real, enc_imag, enc_len, out));
	dec_hard(enc, enc_len, out);
	LOG_BINARY(out, out_len);
}
