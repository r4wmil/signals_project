#ifndef CONV57_H_
#define CONV57_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define ENC_LEN(_l) (((_l) - 2) * 2)

#define DEC_LEN(_l) ((_l) >= 2 ? (2 + (_l) / 2) : 0)

#define LOG_BINARY(arr_, len_) \
	do { \
		for (size_t i = 0; i < (len_); i++) { \
			printf("%d", (arr_)[i]); \
		} \
		printf("\n"); \
	} while(0)

#define LOG_COMPLEX(real_, imag_, len_) \
	do { \
		for (size_t i = 0; i < (len_); i++) { \
			printf("(%.02lf %.02lf), ", (real_)[i], (imag_)[i]); \
		} \
		printf("\n"); \
	} while(0)

void enc(bool* d, size_t dl, size_t ol, double* or, double* oi);
bool dec_no_err(double* dr, double* di, size_t dl, bool* o);

#endif /* CONV57_H_ */

#ifdef CONV57_IMPLEMENTATION

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

bool dec_no_err(double* dr, double* di, size_t dl, bool* o) {
	uint32_t prev = 0;
	uint32_t w = 0;
	size_t j = 0;
	for (size_t i = 0; i < dl; i += 2) {
		uint32_t curr = ((dr[i + 0] > 0) << 1) | (dr[i + 1] > 0);
		uint32_t w0 = ((w << 1) | 0x0) & 0x7;
		uint32_t w1 = ((w << 1) | 0x1) & 0x7;
		uint32_t c0 = (__builtin_parity(w0 & 0x5) % 2 << 1) | __builtin_parity(w0 & 0x7);
		uint32_t c1 = (__builtin_parity(w1 & 0x5)  << 1) | __builtin_parity(w1 & 0x7);
		if (curr == c0) { w = w0; }
		else if (curr == c1) { w = w1; }
		else { return false; }
		o[j++] = (w >> 2) & 0x1;
		prev = curr;
	}
	return true;
}

#endif /* CONV57_IMPLEMENTATION */
