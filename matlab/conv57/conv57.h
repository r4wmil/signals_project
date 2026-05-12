#ifndef CONV57_H_
#define CONV57_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <float.h>
#include <math.h>

typedef struct trel_t {
	uint32_t* window;
	uint32_t* distance;
	uint32_t* backtrack;
	size_t width;
	size_t height;
} trel_t;

typedef struct trel_soft_t {
	uint32_t* window;
	float* distance;
	uint32_t* backtrack;
	size_t width;
	size_t height;
} trel_soft_t;

#define ENC_LEN(_l) (((_l) - 2) * 2)

#define DEC_LEN(_l) ((_l) >= 2 ? (2 + (_l) / 2) : 0)

#define LOG_BINARY(arr_, len_) \
	do { \
		for (size_t i = 0; i < (len_); i++) { \
			printf("%d", (arr_)[i]); \
		} \
		printf("\n"); \
	} while(0)

#define LOG_FLOAT(arr_, len_) \
	do { \
		for (size_t i = 0; i < (len_); i++) { \
			printf("%.02f ", (arr_)[i]); \
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
bool dec_soft(float* d, size_t dl, trel_soft_t* _trel, bool* o);

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

#define LOG_TREL_SOFT(tr_) \
	for (size_t s = 0; s < (tr_).height; s++) { \
		for (size_t t = 0; t < (tr_).width; t++) { \
			size_t i = t * (tr_).height + s; \
			if ((tr_).distance[i] == INT_MAX) { \
				printf("      "); \
				continue; \
			} \
			printf("%01lx(%.02f) ", (tr_).window[i] % 100, (tr_).distance[i] > 100.0f ? 0.0f : (tr_).distance[i]); \
		} \
		printf("\n"); \
	}

trel_t trel_init(size_t width, size_t height) {
	trel_t trel = {0};
	trel.width = width;
	trel.height = height;
	trel.window = calloc(trel.width * trel.height,  sizeof(*trel.window));
	trel.distance = malloc(trel.width * trel.height * sizeof(*trel.distance));
	trel.backtrack = calloc(trel.width * trel.height,  sizeof(*trel.backtrack));
	for (size_t i = 0; i < trel.width * trel.height; i++) {
		trel.distance[i] = INT_MAX;
	}
	assert(trel.window);
	assert(trel.distance);
	return trel;
}

trel_soft_t trel_soft_init(size_t width, size_t height) {
	trel_soft_t trel = {0};
	trel.width = width;
	trel.height = height;
	trel.window = calloc(trel.width * trel.height,  sizeof(*trel.window));
	trel.distance = malloc(trel.width * trel.height * sizeof(*trel.distance));
	trel.backtrack = calloc(trel.width * trel.height,  sizeof(*trel.backtrack));
	for (size_t i = 0; i < trel.width * trel.height; i++) {
		trel.distance[i] = FLT_MAX;
	}
	assert(trel.window);
	assert(trel.distance);
	return trel;
}

bool dec_hard(bool* d, size_t dl, trel_t* _trel, bool* o) {
	trel_t trel = trel_init(dl / 2, 4);
	trel.distance[0] = 0;
	trel.distance[3] = 0;
	trel.window[3] = 7;
	for (size_t i = 0; i < trel.width - 1; i++) {
		uint32_t curr = (d[(i + 1) * 2 + 0] << 1) | d[(i + 1) * 2 + 1];
		for (uint32_t s = 0; s < trel.height; s++) {
			size_t j = i * 2;
			size_t k = i * trel.height + s;
			if (trel.distance[k] == INT_MAX) { continue; }
			uint32_t w = trel.window[k];
			uint32_t w0 = ((w << 1) | 0x0) & 0x7;
			uint32_t w1 = ((w << 1) | 0x1) & 0x7;
			uint32_t c0 = (__builtin_parity(w0 & 0x5) << 1) | __builtin_parity(w0 & 0x7);
			uint32_t c1 = (__builtin_parity(w1 & 0x5) << 1) | __builtin_parity(w1 & 0x7);
			size_t k0 = (i + 1) * trel.height + (w0 & 0x3);
			size_t k1 = (i + 1) * trel.height + (w1 & 0x3);
			uint32_t n0 = trel.distance[k] + __builtin_popcount(curr ^ c0);
			//uint32_t n0 = trel.distance[k] + (curr != c0);
			if (n0 < trel.distance[k0]) {
				trel.window[k0] = w0;
				trel.distance[k0] = n0;
				trel.backtrack[k0] = s;
			}
			uint32_t n1 = trel.distance[k] + __builtin_popcount(curr ^ c1);
			//uint32_t n1 = trel.distance[k] + (curr != c1);
			if (n1 < trel.distance[k1]) {
				trel.window[k1] = w1;
				trel.distance[k1] = n1;
				trel.backtrack[k1] = s;
			}
		}
	}
	uint32_t l = INT_MAX;
	uint32_t b = 0;
	for (uint32_t s = 0; s < trel.height; s++) {
		size_t p = (trel.width - 1) * trel.height + s;
		if (trel.distance[p] < l) {
			b = trel.backtrack[p];
			l = trel.distance[p];
		}
	}
	for (size_t i = trel.width - 2; i != 0; i--) {
		size_t k = i * trel.height + b;
		uint32_t w = trel.window[k];
		o[i] = (w >> 2) & 0x1;
		b = trel.backtrack[k];
	}
	*_trel = trel;
	return true;
}

bool dec_soft(float* d, size_t dl, trel_soft_t* _trel, bool* o) {
	trel_soft_t trel = trel_soft_init(dl / 2, 4);
	trel.distance[0] = 0;
	trel.distance[3] = 0;
	trel.window[3] = 7;
	for (size_t i = 0; i < trel.width - 1; i++) {
		//uint32_t curr = ((d[(i + 1) * 2 + 0] > 0) << 1) | (d[(i + 1) * 2 + 1] > 0);
		float curr0 = d[(i + 1) * 2 + 0];
		float curr1 = d[(i + 1) * 2 + 1];
		float max_dist = FLT_MAX;
		for (uint32_t s = 0; s < trel.height; s++) {
			size_t j = i * 2;
			size_t k = i * trel.height + s;
			if (trel.distance[k] == INT_MAX) { continue; }
			uint32_t w = trel.window[k];
			uint32_t w0 = ((w << 1) | 0x0) & 0x7;
			uint32_t w1 = ((w << 1) | 0x1) & 0x7;
			uint32_t c0 = (__builtin_parity(w0 & 0x5) << 1) | __builtin_parity(w0 & 0x7);
			uint32_t c1 = (__builtin_parity(w1 & 0x5) << 1) | __builtin_parity(w1 & 0x7);
			size_t k0 = (i + 1) * trel.height + (w0 & 0x3);
			size_t k1 = (i + 1) * trel.height + (w1 & 0x3);
			float n0 = trel.distance[k]
				+ fabsf(curr0 - ((float)((c0 << 1) & 0x1) * 2 - 1))
				+ fabsf(curr0 - ((float)((c0 << 0) & 0x1) * 2 - 1));
			//printf("curr0=%f n0=%f\n", curr0, n0);
			//uint32_t n0 = trel.distance[k] + __builtin_popcount(curr ^ c0);
			//uint32_t n0 = trel.distance[k] + (curr != c0);
			if (n0 < trel.distance[k0]) {
				trel.window[k0] = w0;
				trel.distance[k0] = n0;
				trel.backtrack[k0] = s;
			}
			float n1 = trel.distance[k]
				+ fabsf(curr1 - ((float)((c1 << 1) & 0x1) * 2 - 1))
				+ fabsf(curr1 - ((float)((c1 << 0) & 0x1) * 2 - 1));
			//printf("curr1=%f n1=%f\n", curr1, n1);
			//uint32_t n1 = trel.distance[k] + __builtin_popcount(curr ^ c1);
			//uint32_t n1 = trel.distance[k] + (curr != c1);
			if (n1 < trel.distance[k1]) {
				trel.window[k1] = w1;
				trel.distance[k1] = n1;
				trel.backtrack[k1] = s;
			}
			max_dist = trel.distance[k0];
			if (trel.distance[k1] > trel.distance[k0]) {
				max_dist = trel.distance[k1];
			}
		}
		for (uint32_t s = 0; s < trel.height; s++) {
			size_t k = i * trel.height + s;
			trel.distance[k] /= max_dist;
		}
	}
	uint32_t l = INT_MAX;
	uint32_t b = 0;
	for (uint32_t s = 0; s < trel.height; s++) {
		size_t p = (trel.width - 1) * trel.height + s;
		if (trel.distance[p] < l) {
			b = trel.backtrack[p];
			l = trel.distance[p];
		}
	}
	for (size_t i = trel.width - 2; i != 0; i--) {
		size_t k = i * trel.height + b;
		uint32_t w = trel.window[k];
		o[i] = (w >> 2) & 0x1;
		b = trel.backtrack[k];
	}
	*_trel = trel;
	return true;
}

#endif /* CONV57_IMPLEMENTATION */
