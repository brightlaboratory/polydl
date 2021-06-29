

void gemm(int M, int N, int K,
	float  *A, int lda,
	float *B, int ldb,
	float *C, int ldc)
{
	int i, j, k;
	for (i = 0; i < M; ++i) {
		for (k = 0; k < K; ++k) {
			for (j = 0; j < N; ++j) {
				C[i*ldc + j] += A[i*lda + k] * B[k*ldb + j];
			}
		}
	}
}