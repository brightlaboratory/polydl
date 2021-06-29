#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <immintrin.h>
#include <libxsmm.h>


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


#ifndef alpha
#define alpha 1
#endif // !alpha

#ifndef beta
#define beta 1
#endif // !beta

#ifndef NUM_ITERS
#define NUM_ITERS 1000
#endif // !NUM_ITERS



#define TIME
#ifdef TIME
#define IF_TIME(foo) foo;
#else
#define IF_TIME(foo)
#endif


/* function-pointer to LIBXSMM kernel */
libxsmm_smmfunction fwd_gemm;
extern double matmul_high_performance(float A[M1][K1], float B[K1][N1], float C[M1][N1], int iters);

void init_array(float A[M1][K1], float B[K1][N1], float C[M1][N1], float C_ref[M1][N1]) {
	int i, j;

	for (i = 0; i < M1; i++) {
		for (j = 0; j < K1; j++) {
			A[i][j] = ((float)i + (float)j) / (float)(M1 + K1);
		}
	}

	for (i = 0; i < K1; i++) {
		for (j = 0; j < N1; j++) {
			B[i][j] = ((float)i * (float)j) / (float)(K1 + N1);
		}
	}

	for (i = 0; i < M1; i++) {
		for (j = 0; j < N1; j++) {
			C[i][j] = 0.0;
			C_ref[i][j] = 0.0;
		}
	}
}


void matmul_ref(float A[M1][K1], float B[K1][N1], float C[M1][N1]) {
	int i, j, k;
#pragma omp parallel for private(j, k)
	for (i = 0; i < M1; i++)
		for (j = 0; j < N1; j++)
			for (k = 0; k < K1; k++)
				C[i][j] = C[i][j] + A[i][k] * B[k][j];

}

int AreEqual(float* ref, float *result, int num) {
	int i;
	int equal = 1;
	float THRESHOLD = 1.0000001;
	for (i = 0; i < num; i++) {
		if (abs(ref[i] - result[i]) >= THRESHOLD) {
			equal = 0;
			printf("ref[%d] = %f, result[%d] = %f\n", i, ref[i], i, result[i]);
		}
	}

	return equal;
}


void print_array(float C[M1][N1]) {
	int i, j;

	for (i = 0; i < M1; i++) {
		for (j = 0; j < N1; j++) {
			fprintf(stderr, "%lf ", C[i][j]);
			if (j % 80 == 79)
				fprintf(stderr, "\n");
		}
		fprintf(stderr, "\n");
	}
}

double rtclock() {
	struct timezone Tzp;
	struct timeval Tp;
	int stat;
	stat = gettimeofday(&Tp, &Tzp);
	if (stat != 0)
		printf("Error return from gettimeofday: %d", stat);
	return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}
double t_start, t_end;

typedef struct {
	double max_rel_err;
	double max_abs_err;
	double l2_rel_err;
	double one_norm_ref;
	double one_norm_test;
} correctness_t;

void compare_buf(float* ref, float* test, long size, correctness_t* norms)
{
	int i;
	double diff, rel_err;

	norms->max_rel_err = 0.;
	norms->max_abs_err = 0.;
	norms->l2_rel_err = 0.;
	norms->one_norm_ref = 0.;
	norms->one_norm_test = 0.;

	for (i = 0; i < size; ++i) {
		norms->one_norm_ref += (double)ref[i];
		norms->one_norm_test += (double)test[i];
		diff = fabs((double)ref[i] - (double)test[i]);
		norms->l2_rel_err += (diff*diff);
		rel_err = 0.0;
		if (diff > 0.0) {
			rel_err = diff / fabs((double)ref[i]);
		}
		if (rel_err > norms->max_rel_err) {
			norms->max_rel_err = rel_err;
#if 0
			printf("MISMATCH@ %3d: A=%12.8g  B=%12.8g (E:%12.4e) (R:%12.4e)\n", i, ref[i], test[i], diff, rel_err);
#endif
		}
		if (diff > norms->max_abs_err) {
			norms->max_abs_err = diff;
		}
#if 0
		if (diff > 1.0) {
			printf("MISMATCH@ %3d: A=%12.8g  B=%12.8g (E:%12.4e)\n", i, ref[i], test[i], diff);
		}
#endif

	}
	norms->l2_rel_err = sqrt(norms->l2_rel_err);
}

int main() {
	int i, j, k, t;

	correctness_t norms_fwd;
	double l_total;

	// C[M][N] = A[M][K] * B[K][N];
	float(*A)[K1] = (float*)libxsmm_aligned_malloc(M1*K1 * sizeof(float), 2097152);
	float(*B)[N1] = (float*)libxsmm_aligned_malloc(K1*N1 * sizeof(float), 2097152);
	float(*C)[N1] = (float*)libxsmm_aligned_malloc(M1*N1 * sizeof(float), 2097152);
	float(*C_ref)[N1] = (float*)libxsmm_aligned_malloc(M1*N1 * sizeof(float), 2097152);

	int N1_val = N1;
	int M1_val = M1;
	int K1_val = K1;

	printf("M1 = %d, N1 = %d, K1 = %d\n", M1, N1, K1);
	printf("M1_Tile = %d, N1_Tile = %d, K1_Tile = %d\n", M1_Tile, N1_Tile, K1_Tile);
	printf("SIZE A  (MB): %10.2f MB\n", (double)(M1*K1 * sizeof(float)) / (1024.0*1024.0));
	printf("SIZE B  (MB): %10.2f MB\n", (double)(K1*N1 * sizeof(float)) / (1024.0*1024.0));
	printf("SIZE C  (MB): %10.2f MB\n", (double)(M1*N1 * sizeof(float)) / (1024.0*1024.0));

	if (M1 % M2_Tile != 0 || N1 % N2_Tile != 0 || K1 % K2_Tile != 0) {
		printf("X2_Tile sizes do not divide the problem sizes\n");
		exit(1);
	}


	if (M2_Tile % M1_Tile != 0 || N2_Tile % N1_Tile != 0 || K2_Tile % K1_Tile != 0) {
		printf("X1_Tile sizes do not divide X2_Tile sizes\n");
		exit(1);
	}

	fwd_gemm = libxsmm_smmdispatch(N1_Tile, M1_Tile, K1_Tile,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	init_array(A, B, C, C_ref);

#ifndef BASELINE
	matmul_ref(A, B, C);
	matmul_high_performance(A, B, C_ref, 1);

	/* compare */
	compare_buf(C_ref, C, M1*N1, &norms_fwd);
	printf("             1-norm of reference: %f\n", norms_fwd.one_norm_ref);
	printf("             1-norm of GEMM-code: %f\n", norms_fwd.one_norm_test);
	printf("      L2-error-norm of GEMM-code: %f\n", norms_fwd.l2_rel_err);
	printf("    inf-norm of comp. rel. error: %f\n", norms_fwd.max_rel_err);
	printf("    inf-norm of comp. abs. error: %f\n", norms_fwd.max_abs_err);

	/*
	if (AreEqual(C_ref, C, M1*N1) == 0) {
		printf("Correctness check failed. Exiting\n");
		// exit(1);
	}
	else {
		printf("Correctness check passed.\n");
	}
	*/

	printf("A: %f, %f\n", A[0][0], A[M1 - 1][K1 - 1]);
	printf("B: %f, %f\n", B[0][0], B[K1 - 1][N1 - 1]);
	printf("C_ref: %f, %f, %f\n", C_ref[0][0], C_ref[M1 / 2][N1 / 2], C_ref[M1 - 1][N1 - 1]);
	printf("C: %f, %f, %f\n", C[0][0], C[M1 / 2][N1 / 2], C[M1 - 1][N1 - 1]);

	init_array(A, B, C, C_ref);
	l_total = matmul_high_performance(A, B, C, NUM_ITERS);
	printf("Total time in seconds: %f\n", l_total);
#endif

#ifdef BASELINE
	unsigned long long l_start, l_end;
	l_start = libxsmm_timer_tick();
	for (int iter = 0; iter < NUM_ITERS; iter++) {
		matmul_ref(A, B, C);
	}

	l_end = libxsmm_timer_tick();
	l_total = libxsmm_timer_duration(l_start, l_end);
#endif

	double flops = NUM_ITERS * 2.0 * M1 * N1 * K1;
	printf("Real_GFLOPS =%0.2lf\n",
		(flops*1e-9) / l_total);

	libxsmm_free(A);
	libxsmm_free(B);
	libxsmm_free(C);
	libxsmm_free(C_ref);

	return 0;
}
