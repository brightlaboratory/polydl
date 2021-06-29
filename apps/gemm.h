#ifndef GEMM_H
#define GEMM_H

void gemm(int M, int N, int K,
	float  *A, int lda,
	float *B, int ldb,
	float *C, int ldc);

#endif