#ifndef M1
#define M1 4096
#endif // !M1

#ifndef N1
#define N1 8192
#endif // !N1

#ifndef K1
#define K1 16384
#endif // !K1


#ifndef M2_Tile
#define M2_Tile 512
#endif // !M2_Tile

#ifndef N2_Tile
#define N2_Tile 1024
#endif // !N2_Tile

#ifndef K2_Tile
#define K2_Tile 2048
#endif // !K2_Tile

#ifndef M1_Tile
#define M1_Tile 64
#endif // !M1_Tile

#ifndef N1_Tile
#define N1_Tile 64
#endif // !N1_Tile

#ifndef K1_Tile
#define K1_Tile 64
#endif // !K1_Tile


#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

double matmul_high_performance_scop(float A[M1][K1], float B[K1][N1], float C[M1][N1], int iters)
{
	int it2, jt2, kt2, it1, jt1, kt1, i, j, k;

#pragma scop
	// First level of tiling
#pragma omp parallel for private(it2, kt2, it1, jt1, kt1, i, j, k)
	for (jt2 = 0; jt2 < N1; jt2 += N2_Tile) {
		for (kt2 = 0; kt2 < K1; kt2 += K2_Tile) {
			for (it2 = 0; it2 < M1; it2 += M2_Tile) {


				// Second level of tiling
				for (jt1 = jt2; jt1 < min(N1, jt2 + N2_Tile); jt1 += N1_Tile) {
					for (kt1 = kt2; kt1 < min(K1, kt2 + K2_Tile); kt1 += K1_Tile) {
						for (it1 = it2; it1 < min(M1, it2 + M2_Tile); it1 += M1_Tile) {

							// Inner most intra-tile loops
							for (i = it1; i < min(M1, it1 + M1_Tile); i++) {
								for (j = jt1; j < min(N1, jt1 + N1_Tile); j++) {
									for (k = kt1; k < min(K1, kt1 + K1_Tile); k++) {
										C[i][j] = C[i][j] + A[i][k] * B[k][j];
									}
								}
							}
						}
					}
				}
			}
		}
	}
#pragma endscop

	return 1;
}

#ifdef USE_LIBXSMM
#include <libxsmm.h>
extern libxsmm_smmfunction fwd_gemm;

void copyToTiledArray(int SIZE1, int SIZE2, int T1, int T2,
	float A[SIZE1][SIZE2], float A_Tiled[SIZE1 / T1][SIZE2 / T2][T1][T2]) {
	int it, jt, i, j;
	for (it = 0; it < SIZE1 / T1; it++) {
		for (jt = 0; jt < SIZE2 / T2; jt++) {
			for (i = 0; i < T1; i++) {
				for (j = 0; j < T2; j++) {
					A_Tiled[it][jt][i][j] = A[it*T1 + i][jt*T2 + j];
				}
			}
		}
	}
}

void copyFromTiledArray(int SIZE1, int SIZE2, int T1, int T2,
	float A[SIZE1][SIZE2], float A_Tiled[SIZE1 / T1][SIZE2 / T2][T1][T2]) {
	int it, jt, i, j;
	for (it = 0; it < SIZE1 / T1; it++) {
		for (jt = 0; jt < SIZE2 / T2; jt++) {
			for (i = 0; i < T1; i++) {
				for (j = 0; j < T2; j++) {
					A[it*T1 + i][jt*T2 + j] = A_Tiled[it][jt][i][j];
				}
			}
		}
	}
}

double matmul_high_performance(float A[M1][K1], float B[K1][N1], float C[M1][N1], int iters) {
	unsigned long long l_start, l_end;
	double l_total = 0.0;
	int i;
	printf("In matmul5.c\n");
	printf("M1_Tile = %d, N1_Tile = %d, K1_Tile = %d\n", M1_Tile, N1_Tile, K1_Tile);
	printf("M2_Tile = %d, N2_Tile = %d, K2_Tile = %d\n", M2_Tile, N2_Tile, K2_Tile);

	float(*A_Tiled)[K1 / K1_Tile][M1_Tile][K1_Tile] =
		(float*)libxsmm_aligned_malloc(M1*K1 * sizeof(float), 2097152);
	float(*B_Tiled)[N1 / N1_Tile][K1_Tile][N1_Tile] =
		(float*)libxsmm_aligned_malloc(N1*K1 * sizeof(float), 2097152);
	float(*C_Tiled)[N1 / N1_Tile][M1_Tile][N1_Tile] =
		(float*)libxsmm_aligned_malloc(M1*N1 * sizeof(float), 2097152);

	copyToTiledArray(M1, K1, M1_Tile, K1_Tile, A, A_Tiled);
	copyToTiledArray(K1, N1, K1_Tile, N1_Tile, B, B_Tiled);
	copyToTiledArray(M1, N1, M1_Tile, N1_Tile, C, C_Tiled);

	l_start = libxsmm_timer_tick();

	for (i = 0; i < iters; i++) {
		matmul_high_performance_core(A_Tiled, B_Tiled, C_Tiled);
	}

	l_end = libxsmm_timer_tick();
	l_total = libxsmm_timer_duration(l_start, l_end);

	copyFromTiledArray(M1, N1, M1_Tile, N1_Tile, C, C_Tiled);
	libxsmm_free(A_Tiled);
	libxsmm_free(B_Tiled);
	libxsmm_free(C_Tiled);

	return l_total;
}

void matmul_high_performance_core(
	float A[M1 / M1_Tile][K1 / K1_Tile][M1_Tile][K1_Tile],
	float B[K1 / K1_Tile][N1 / N1_Tile][K1_Tile][N1_Tile],
	float C[M1 / M1_Tile][N1 / N1_Tile][M1_Tile][N1_Tile])
{
	int it2, jt2, kt2, it1, jt1, kt1, i, j, k;

	// #pragma omp parallel for private(it2, kt2, it1, jt1, kt1, i, j, k)
	for (jt2 = 0; jt2 < N1; jt2 += N2_Tile) {
		for (kt2 = 0; kt2 < K1; kt2 += K2_Tile) {
			// #pragma omp parallel for private(it1, jt1, kt1, i, j, k)
			for (it2 = 0; it2 < M1; it2 += M2_Tile) {

				// Second level of tiling
// #pragma omp parallel for private(it1, kt1, i, j, k)
				for (jt1 = jt2; jt1 < min(N1, jt2 + N2_Tile); jt1 += N1_Tile) {
					for (kt1 = kt2; kt1 < min(K1, kt2 + K2_Tile); kt1 += K1_Tile) {
#pragma omp parallel for private(i, j, k)
						for (it1 = it2; it1 < min(M1, it2 + M2_Tile); it1 += M1_Tile) {
							fwd_gemm(&B[kt1 / K1_Tile][jt1 / N1_Tile][0][0],
								&A[it1 / M1_Tile][kt1 / K1_Tile][0][0],
								&C[it1 / M1_Tile][jt1 / N1_Tile][0][0]);
						}
					}
				}
			}
		}
	}


}

#endif