#ifndef CONV57_H_
#define CONV57_H_

#include <stdio.h>
#include <stdbool.h>

#define ENC_LEN(_l) (((_l) - 2) * 2)

#define LOG_COMPLEX(real_, imag_, len_) \
	do { \
		for (size_t i = 0; i < (len_); i++) { \
			printf("(%.02lf %.02lf), ", (real_)[i], (imag_)[i]); \
		} \
		printf("\n"); \
	} while(0)

void enc(bool* d, size_t dl, size_t ol, double* or, double* oi);

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


#endif /* CONV57_IMPLEMENTATION */
