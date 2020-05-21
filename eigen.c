/*
 * eigen.c
 *
 *  Created on: Apr 8, 2020
 *      Author: Muhammad & Shadi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include "SPBufferset.h"

#define EPSILON 0.00001

void calculate_next(double *next, double *prev, FILE *cov, int n, double *auxArray);
double calculate_entry(double *row, double *prev, int n);
void init_vector(double *vector, int n);
int is_good_approx(double *b_prev, double *b_next, int n);

int main(int argc, char **argv) {

	FILE *cov, *vector;
	clock_t start, end;
	register double *b_prev, *b_next, *temp;
	double *auxArray;
	register int res;
	int vals[2], n;

	assert(cov == cov);
	assert(vector == vector);

	start = clock();
	SP_BUFF_SET();
	srand(time(NULL));

	assert(argc == 3);


	cov = fopen(argv[1], "r");
	assert(cov != NULL);

	res = fread(&n, sizeof(int), 1, cov);
	assert(res == 1);

	b_prev = (double*) malloc(n * sizeof(double));
	assert(b_prev != NULL);

	init_vector(b_prev, n);

	b_next = (double*) malloc(n * sizeof(double));
	assert(b_next != NULL);

	vector = fopen(argv[2], "w");
	assert(vector != NULL);

	vals[0] = 1;
	vals[1] = n;
	res = fwrite(vals, sizeof(int), 2, vector);
	assert(res == 2);

	auxArray = (double*) malloc(n * sizeof(double));
	assert(auxArray != NULL);

	while(1){

		calculate_next(b_next, b_prev, cov, n, auxArray);

		if(is_good_approx(b_prev, b_next, n)){

			res = fwrite(b_next, sizeof(double), n, vector);
			assert(res == n);
			break;
		}

		temp = b_prev;
		b_prev = b_next;
		b_next = temp;

	}


	free(b_prev);
	free(b_next);
	free(auxArray);
	fclose(vector);
	fclose(cov);
	end = clock();
	printf("Eigen took: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

	return 0;

}

void init_vector(double *vector, int n){

	register int i;

	for(i = 0; i < n; i++)
		vector[i] = rand();

}

void calculate_next(double *next, double *prev, FILE *cov, int n, double *auxArray){

	register int i;
	int res;
	register double *ptr_1;
	register long double norm;

	norm = 0;

	res = fseek(cov, 2 * sizeof(int), SEEK_SET);
	assert(res == 0);

	ptr_1 = next;

	for(i = 0; i < n; i++){
		fread(auxArray, sizeof(double), n, cov);
		*ptr_1 = calculate_entry(auxArray, prev, n);
		norm += (long double)(*ptr_1) * (long double)(*ptr_1);
		ptr_1++;
	}

	norm = sqrt(norm);

	ptr_1 = next;

	for(i = 0; i < n; i++){
		*ptr_1 = (*ptr_1) / norm;
		ptr_1++;
	}

	rewind(cov);

}

double calculate_entry(double *row, double *prev, int n){

	register int i;
	register double sum;
	register double *ptr_1, *ptr_2;

	sum = 0;

	ptr_1 = row;
	ptr_2 = prev;

	for(i = 0; i < n; i++){
		sum += (*ptr_1) * (*ptr_2);
		ptr_1++;
		ptr_2++;
	}

	return sum;

}

int is_good_approx(double *b_prev, double *b_next, int n){

	register int i;
	register double *ptr_1, *ptr_2;

	ptr_1 = b_prev;
	ptr_2 = b_next;

	for(i = 0; i < n; i++){
		if(fabs((*ptr_1)-(*ptr_2)) >= EPSILON)
			return 0;
		ptr_1++;
		ptr_2++;
	}

	return 1;
}
