/*
 * cov.c
 *
 *  Created on: Apr 8, 2020
 *      Author: Muhammad & Shadi
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SPBufferset.h"

void calculate_avg(int n_cols, int n_rows, double* matrix, double* avgs);
double calculate_cov(int i, double avg_i, int j, double avg_j, double *input_matrix, int n_cols);

int main(int argc, char **argv) {

	FILE *input, *output;
	register int n_cols, n_rows, size, res, i, j;
	register double *input_matrix, *avgs, *row, *ptr_1, *ptr_2, *ptr_3;
	clock_t start, end;
	int n_cols_rows[2];

	start = clock();

	SP_BUFF_SET();
	assert(argc == 3);

	input = fopen(argv[1], "r");
	assert(input != NULL);

	res = fread(n_cols_rows, sizeof(int), 2, input);
	assert(res == 2);

	n_cols = n_cols_rows[0];
	n_rows = n_cols_rows[1];

	size = n_cols * n_rows;

	input_matrix = (double*) malloc(size * sizeof(double));
	assert(input_matrix != NULL);

	res = fread(input_matrix, sizeof(double), size, input);
	assert(res == n_cols * n_rows);

	avgs = (double*) malloc(n_rows * sizeof(double));
	assert(input_matrix != NULL);

	calculate_avg(n_cols, n_rows, input_matrix, avgs);

	output = fopen(argv[2], "w");
	assert(output != NULL);

	n_cols_rows[0] = n_rows;
	n_cols_rows[1] = n_rows;

	res = fwrite(n_cols_rows, sizeof(int), 2, output);
	assert(res == 2);

	row = malloc(n_rows * sizeof(double));
	assert(row != NULL);

	ptr_3 = avgs;

	for(i = 0; i < n_rows; i++){

		ptr_1 = row;
		ptr_2 = avgs;

		for(j = 0; j < n_rows; j++){
			*ptr_1 = calculate_cov(i, *ptr_3, j, *ptr_2, input_matrix, n_cols);
			ptr_1++;
			ptr_2++;
		}

		res = fwrite(row, sizeof(double), n_rows, output);
		assert(res == n_rows);

	}

	free(input_matrix);
	free(avgs);
	free(row);

	fclose(input);
	fclose(output);

	end = clock();

	printf("Cov   took: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

	return 0;

}


double calculate_cov(int i, double avg_i, int j, double avg_j, double *input_matrix, int n_cols){

	register int k;
	register double *ptr_1, *ptr_2, product;

	ptr_1 = input_matrix + i * n_cols;
	ptr_2 = input_matrix + j * n_cols;

	product = 0;

	for(k = 0; k < n_cols; k++){
		product += (*ptr_1 - avg_i) * (*ptr_2 - avg_j);
		ptr_1++;
		ptr_2++;
	}

	return product;

}

void calculate_avg(int n_cols, int n_rows, double* matrix, double* avgs){

	register int i, size;
	register double sum;
	register double *ptr;

	sum = 0;
	size = n_cols * n_rows;

	ptr = matrix;

	for(i = 0; i < size; i++){

		sum += *ptr;

		if((i+1) % n_cols == 0){
			*avgs = sum / n_cols;
			sum = 0;
			avgs++;
		}

		ptr++;
	}

}
