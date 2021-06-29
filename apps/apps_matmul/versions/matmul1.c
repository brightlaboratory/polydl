#ifndef M1
#define M1 32
#endif // !M1

#ifndef N1
#define N1 32
#endif // !N1

#ifndef K1
#define K1 32
#endif // !K1


#ifdef USE_LIBXSMM
#include <libxsmm.h>
extern libxsmm_smmfunction fwd_gemm;

double matmul_high_performance(float A[M1][K1], float B[K1][N1], float C[M1][N1], int iters)
{
	int i;
	for (i = 0; i < iters; i++) {
		fwd_gemm(&B[0][0], &A[0][0], &C[0][0]);
	}

	return 1;
}

#endif