void matmul(int M, int N, int K, float A[M][K], float B[K][N], float C[M][N]) {

	int i, j, k;

#pragma scop
#pragma omp parallel for private(i, k)
	for (j = 0; j < N; j++) {
		for (i = 0; i < M; i++) {
			for (k = 0; k < K; k++) {
				C[i][j] = C[i][j] + A[i][k] * B[k][j];
			}
		}
	}

#pragma endscop

}
