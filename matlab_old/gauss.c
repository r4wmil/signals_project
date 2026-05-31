#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double randg(double sigma) {
	static double spare = 0.0;
	if (spare != INFINITY) {
		double result = sigma * spare;
		spare = INFINITY;
		return result;
	}
	double r = sqrt(-2.0 * log((double)rand() / RAND_MAX));
	double a = 2.0 * M_PI * ((double)rand() / RAND_MAX);
	spare = r * sin(a);
	return sigma * (r * cos(a));
}

int main() {
	for (size_t i = 0; i < 1000; i++)
		printf("%lf ", randg(1.0));
	printf("\n");
	return 0;
}
