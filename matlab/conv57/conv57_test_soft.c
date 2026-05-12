#include "conv57.c"

int main() {
	bool inp[] = {0,0,0,1,1,0,1,0,0,0};
	bool out[] = {0,0,0,0,0,0,0,0,0,0};
	size_t inp_len = sizeof(inp)/sizeof(*inp);
	size_t out_len = sizeof(out)/sizeof(*out);
	LOG_BINARY(inp, inp_len);
	const size_t enc_len = ENC_LEN(inp_len);
	bool encb[enc_len];
	float enc[enc_len];
	enc_hard(inp, inp_len, encb);
	for (size_t i = 0; i < enc_len; i++) {
		enc[i] = 2.0f * (float)encb[i] - 1.0f;
	}
	LOG_BINARY(encb, enc_len);
	enc[5] = +0.5;
	enc[6] = -0.3;
	enc[7] = -0.5;
	LOG_FLOAT(enc, enc_len);
	//printf("ok=%d\n", dec_no_err(enc_real, enc_imag, enc_len, out));
	trel_soft_t trel;
	dec_soft(enc, enc_len, &trel, out);
	LOG_BINARY(out, out_len);
	LOG_TREL_SOFT(trel);
}
