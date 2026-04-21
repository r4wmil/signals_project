#ifndef CONV57_H_
#define CONV57_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

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
		uint32_t c0 = (__builtin_parity(w0 & 0x5) << 1) | __builtin_parity(w0 & 0x7);
		uint32_t c1 = (__builtin_parity(w1 & 0x5) << 1) | __builtin_parity(w1 & 0x7);
		if (curr == c0) { w = w0; }
		else if (curr == c1) { w = w1; }
		else { return false; }
		o[j++] = (w >> 2) & 0x1;
		prev = curr;
	}
	return true;
}

#define LOG_TREL(trw_, trl_, s_, l_) \
	for (size_t s = 0; s < (s_); s++) { \
		for (size_t t = 0; t < (l_); t++) { \
			size_t i = t * (s_) + s; \
			if (trl[i] == INT_MAX) { \
				printf("      "); \
				continue; \
			} \
			printf("%01lx(%02lu) ", (trw_)[i] % 100, trl[i] % 100); \
		} \
		printf("\n"); \
	}

bool dec_hard(double* dr, double* di, size_t dl, bool* o) {
	// TODO: 1 less than dl / 2
	uint32_t* trw = calloc(dl * 4,  sizeof(*trw));
	uint32_t* trl = malloc(dl * 4 * sizeof(*trl));
	for (size_t i = 0; i < dl * 4; i++) {
		trl[i] = INT_MAX;
	}
	trl[0] = 0;
	for (size_t i = 0; i < dl / 2 - 1; i++) {
		uint32_t curr = ((dr[(i + 1) * 2 + 0] > 0) << 1) | (dr[(i + 1) * 2 + 1] > 0);
		for (uint32_t s = 0; s < 4; s++) {
			size_t j = i * 2;
			size_t k = i * 4 + s;
			if (trl[k] == INT_MAX) { continue; }
			uint32_t w = trw[k];
			uint32_t w0 = ((w << 1) | 0x0) & 0x7;
			uint32_t w1 = ((w << 1) | 0x1) & 0x7;
			uint32_t s0 = (__builtin_parity(w0 & 0x5) << 1) | __builtin_parity(w0 & 0x7);
			uint32_t s1 = (__builtin_parity(w1 & 0x5) << 1) | __builtin_parity(w1 & 0x7);
			size_t k0 = (i + 1) * 4 + s0;
			size_t k1 = (i + 1) * 4 + s1;
			uint32_t n0 = trl[k] + (curr != s0);
			if (n0 < trl[k0]) {
				trw[k0] = w0;
				trl[k0] = n0;
			}
			uint32_t n1 = trl[k] + (curr != s1);
			if (n1 < trl[k1]) {
				trw[k1] = w1;
				trl[k1] = n1;
			}
		}
	}
	for (size_t i = dl / 2 - 1; i != 0; i--) {
		size_t k = i * 4 + 0;
		uint32_t w = trw[k];
		uint32_t l = trl[k];
		for (uint32_t s = 0; s < 4; s++) {
			size_t p = i * 4 + s;
			if (trl[p] < l) {
				w = trw[p];
				l = trl[p];
			}
		}
		o[i] = (w >> 2) & 0x1;
	}
	LOG_TREL(trw, trl, 4, dl);
	return true;
}

#endif /* CONV57_IMPLEMENTATION */
