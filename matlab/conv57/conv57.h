#ifndef CONV57_H_
#define CONV57_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

typedef struct trel_t {
	uint32_t* window;
	uint32_t* distance;
	size_t width;
	size_t height;
} trel_t;

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

void enc_hard(bool* d, size_t dl, bool* o);
bool dec_no_err(bool* d, size_t dl, bool* o);
bool dec_hard(bool* d, size_t dl, trel_t* trel, bool* o);

#endif /* CONV57_H_ */

#ifdef CONV57_IMPLEMENTATION

void enc_hard(bool* d, size_t dl, bool* o) {
	size_t j = 0;
	for (size_t i = 0; i < dl - 2; i++) {
		o[j + 0] = d[i + 0]            ^ d[i + 2];
		o[j + 1] = d[i + 0] ^ d[i + 1] ^ d[i + 2];
		j += 2;
	}
}

bool dec_no_err(bool* d, size_t dl, bool* o) {
	uint32_t prev = 0;
	uint32_t w = 0;
	size_t j = 0;
	for (size_t i = 0; i < dl; i += 2) {
		uint32_t curr = (d[i + 0] << 1) | d[i + 1];
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

#define LOG_TREL(tr_) \
	for (size_t s = 0; s < (tr_).height; s++) { \
		for (size_t t = 0; t < (tr_).width; t++) { \
			size_t i = t * (tr_).height + s; \
			if ((tr_).distance[i] == INT_MAX) { \
				printf("      "); \
				continue; \
			} \
			printf("%01lx(%02lu) ", (tr_).window[i] % 100, (tr_).distance[i] % 100); \
		} \
		printf("\n"); \
	}

trel_t trel_init(size_t width, size_t height) {
	trel_t trel = {0};
	trel.width = width;
	trel.height = height;
	trel.window = calloc(trel.width * trel.height,  sizeof(*trel.window));
	trel.distance = malloc(trel.width * trel.height * sizeof(*trel.distance));
	for (size_t i = 0; i < trel.width * trel.height; i++) {
		trel.distance[i] = INT_MAX;
	}
	assert(trel.window);
	assert(trel.distance);
	return trel;
}

bool dec_hard(bool* d, size_t dl, trel_t* _trel, bool* o) {
	trel_t trel = trel_init(dl / 2, 4);
	trel.distance[0] = 0;
	for (size_t i = 0; i < trel.width - 1; i++) {
		uint32_t curr = (d[(i + 1) * 2 + 0] << 1) | d[(i + 1) * 2 + 1];
		for (uint32_t s = 0; s < trel.height; s++) {
			size_t j = i * 2;
			size_t k = i * trel.height + s;
			if (trel.distance[k] == INT_MAX) { continue; }
			uint32_t w = trel.window[k];
			uint32_t w0 = ((w << 1) | 0x0) & 0x7;
			uint32_t w1 = ((w << 1) | 0x1) & 0x7;
			uint32_t s0 = (__builtin_parity(w0 & 0x5) << 1) | __builtin_parity(w0 & 0x7);
			uint32_t s1 = (__builtin_parity(w1 & 0x5) << 1) | __builtin_parity(w1 & 0x7);
			size_t k0 = (i + 1) * trel.height + s0;
			size_t k1 = (i + 1) * trel.height + s1;
			uint32_t n0 = trel.distance[k] + (curr != s0);
			if (n0 < trel.distance[k0]) {
				trel.window[k0] = w0;
				trel.distance[k0] = n0;
			}
			uint32_t n1 = trel.distance[k] + (curr != s1);
			if (n1 < trel.distance[k1]) {
				trel.window[k1] = w1;
				trel.distance[k1] = n1;
			}
		}
	}
	for (size_t i = trel.width - 1; i != 0; i--) {
		size_t k = i * trel.height + 0;
		uint32_t w = trel.window[k];
		uint32_t l = trel.distance[k];
		for (uint32_t s = 0; s < trel.height; s++) {
			size_t p = i * trel.height + s;
			if (trel.distance[p] < l) {
				w = trel.window[p];
				l = trel.distance[p];
			}
		}
		o[i] = (w >> 2) & 0x1;
	}
	*_trel = trel;
	return true;
}

#endif /* CONV57_IMPLEMENTATION */
