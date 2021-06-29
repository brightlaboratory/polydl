#ifndef M1
#define M1 32
#endif // !M1

#ifndef N1
#define N1 32
#endif // !N1

#ifndef K1
#define K1 32
#endif // !K1


#ifndef M2_Tile
#define M2_Tile M1
#endif // !M2_Tile

#ifndef N2_Tile
#define N2_Tile N1
#endif // !N2_Tile

#ifndef K2_Tile
#define K2_Tile K1
#endif // !K2_Tile

#ifndef M1_Tile
#define M1_Tile M2_Tile
#endif // !M1_Tile

#ifndef N1_Tile
#define N1_Tile N2_Tile
#endif // !N1_Tile

#ifndef K1_Tile
#define K1_Tile K2_Tile
#endif // !K1_Tile


#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

double matmul_high_performance_scop(float A[M1][K1], float B[K1][N1], float C[M1][N1], int iters)
{
	int it2, jt2, kt2, it1, jt1, kt1, i, j, k;
	// printf("In matmul2\n");
#pragma scop
	// First level of tiling
	for (it2 = 0; it2 < M1; it2 += M2_Tile) {
		for (jt2 = 0; jt2 < N1; jt2 += N2_Tile) {
			for (kt2 = 0; kt2 < K1; kt2 += K2_Tile) {

				// Second level of tiling
				for (it1 = it2; it1 < min(M1, it2 + M2_Tile); it1 += M1_Tile) {
					for (jt1 = jt2; jt1 < min(N1, jt2 + M2_Tile); jt1 += N1_Tile) {
						for (kt1 = kt2; kt1 < min(K1, kt2 + K2_Tile); kt1 += K1_Tile) {

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

double matmul_high_performance(float A[M1][K1], float B[K1][N1], float C[M1][N1], int iters) {
	unsigned long long l_start, l_end;
	double l_total = 0.0;
	int i;
	printf("In matmul2.c\n");
	l_start = libxsmm_timer_tick();

	for (i = 0; i < iters; i++) {
		matmul_high_performance_core(A, B, C);
	}

	l_end = libxsmm_timer_tick();
	l_total = libxsmm_timer_duration(l_start, l_end);
	return l_total;
}

void matmul_high_performance_core(float A[M1][K1], float B[K1][N1], float C[M1][N1])
{
	int it2, jt2, kt2, it1, jt1, kt1, i, j, k;

#pragma omp parallel for private(jt2, kt2, it1, jt1, kt1, i, j, k)
	for (it2 = 0; it2 < M1; it2 += M2_Tile) {
		for (jt2 = 0; jt2 < N1; jt2 += N2_Tile) {
			for (kt2 = 0; kt2 < K1; kt2 += K2_Tile) {

				// Second level of tiling
				for (it1 = it2; it1 < min(M1, it2 + M2_Tile); it1 += M1_Tile) {
					for (jt1 = jt2; jt1 < min(N1, jt2 + M2_Tile); jt1 += N1_Tile) {
						for (kt1 = kt2; kt1 < min(K1, kt2 + K2_Tile); kt1 += K1_Tile) {

							float ATile[M1_Tile][K1_Tile];
							float BTile[K1_Tile][N1_Tile];
							float CTile[M1_Tile][N1_Tile];
							// Inner most intra-tile loops
							for (i = it1; i < min(M1, it1 + M1_Tile); i++) {
								for (j = jt1; j < min(N1, jt1 + N1_Tile); j++) {
									CTile[i - it1][j - jt1] = C[i][j];
								}
							}

							for (i = it1; i < min(M1, it1 + M1_Tile); i++) {
								for (k = kt1; k < min(K1, kt1 + K1_Tile); k++) {
									ATile[i - it1][k - kt1] = A[i][k];
								}
							}

							for (j = jt1; j < min(N1, jt1 + N1_Tile); j++) {
								for (k = kt1; k < min(K1, kt1 + K1_Tile); k++) {
									BTile[k - kt1][j - jt1] = B[k][j];
								}
							}


							fwd_gemm(&BTile[0][0], &ATile[0][0], &CTile[0][0]);


							for (i = it1; i < min(M1, it1 + M1_Tile); i++) {
								for (j = jt1; j < min(N1, jt1 + N1_Tile); j++) {
									C[i][j] = CTile[i - it1][j - jt1];
								}
							}

						}
					}
				}
			}
		}
	}


}

#endif