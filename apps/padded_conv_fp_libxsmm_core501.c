#ifndef STRIDE_H
#define STRIDE_H 1
#endif // !STRIDE_H

#ifndef STRIDE_W
#define STRIDE_W 1
#endif // !STRIDE_W

#ifndef GEMM_BLOCK
#define GEMM_BLOCK 64
#endif // !GEMM_BLOCK.

#ifndef T_ofm
#define T_ofm 64
#endif // !T_ofm

#ifndef T_ifm
#define T_ifm 64
#endif // !T_ifm

void padded_conv_fp_libxsmm_core501_gemm(int nImg, int nIfm, int nOfm, int ifhp, int ifwp, int ofhp, int ofwp, int ifh, int ifw,
	int ofh, int ofw, int pad_h, int pad_w, int pad_h_in, int pad_w_in, int pad_h_out,
	int pad_w_out, int kh, int kw, int stride_h, int stride_w,
	const float input[nImg][nIfm][ifhp + 2 * pad_h][ifwp + 2 * pad_w], float output[nImg][nOfm][ofhp][ofwp], const float filter[nOfm][nIfm][kh][kw]
/*,int iters*/)
{
	/* loop counters */
	int img, ofm, ifm, oj, oi, ij, ii, kj, ki, t_ofm, t_ifm;

	/*
float input[nImg][nIfm][ifhp][ifwp];
float output[nImg][nOfm][ofhp][ofwp];
float filter[nOfm][nIfm][kh][kw];
*/


// printf("lets see if i work?");
// float(*C)[T_ofm] = malloc(sizeof (T_ofm * ofw) * sizeof(float));
// float(*B)[T_ifm] = malloc (sizeof (T_ifm * ofw) * sizeof(float));
// float(*A)[T_ofm] = malloc (sizeof (T_ofm * T_ifm) * sizeof(float));
#pragma omp parallel for private(ofm, ifm,t_ofm, t_ifm,oj, ij, oi, ii, kj, ki)

	for (img = 0; img < nImg; ++img) {
		float C[T_ofm][ofw];
		float B[T_ifm][ofw];
		float A[T_ofm][T_ifm];
		for (t_ofm = 0; t_ofm < nOfm; t_ofm += T_ofm) {
			for (t_ifm = 0; t_ifm < nIfm; t_ifm += T_ifm) {
				for (oj = 0; oj < ofh; ++oj) {
					ij = oj * stride_h;
					for (kj = 0; kj < kh; ++kj) {
						for (ki = 0; ki < kw; ++ki) {

							// Pack 
							// Packing code for C
							for (oi = 0; oi < ofw; ++oi) /*j loop */ {
								ii = oi * stride_w;
								for (ofm = t_ofm; ofm < min(nOfm, t_ofm + T_ofm); ofm++) /*i loop */ {
									C[ofm - t_ofm][oi - 0] = output[img][ofm][oj][oi];
									// C[0][0], C[0][1] ... = output[img][ofm][oj][oi], output[img][ofm+1][oj][oi+1]
								}
							}

							// 	//Packing code for B
							for (oi = 0; oi < ofw; ++oi) /*j loop */ {
								ii = oi * stride_w;
								for (ifm = t_ifm; ifm < min(nIfm, t_ifm + T_ifm); ifm++) /*i loop */ {
									B[ifm - t_ifm][oi - 0] = input[img][ifm][ij + kj][ii + ki];
									// C[0][0], C[0][1] ... = output[img][ofm][oj][oi], output[img][ofm+1][oj][oi+1]
								}
							}

							// 	//Packing code for A
							for (ofm = t_ofm; ofm < min(nOfm, t_ofm + T_ofm); ofm++) /*j loop */ {
								for (ifm = t_ifm; ifm < min(nIfm, t_ifm + T_ifm); ifm++) /*i loop */ {
									A[ofm - t_ofm][ifm - t_ifm] = filter[ofm][ifm][kj][ki];
									// C[0][0], C[0][1] ... = output[img][ofm][oj][oi], output[img][ofm+1][oj][oi+1]
								}
							}

							//Gemm 
							for (oi = 0; oi < ofw; ++oi) /*j loop */ {
								ii = oi * stride_w;
								for (ofm = t_ofm; ofm < min(nOfm, t_ofm + T_ofm); ofm++) /*i loop */ {
									for (ifm = t_ifm; ifm < min(nIfm, t_ifm + T_ifm); ifm++) /* k loop */ {
										// output[img][ofm][oj][oi] +=
										// filter[ofm][ifm][kj][ki] /* filter - A */ * input[img][ifm][ij + kj][ii + ki] /*input - B */;
										C[ofm - t_ofm][oi] += A[ofm - t_ofm][ifm - t_ifm] * B[ifm - t_ifm][oi];

									}
								}
							}
							// fwd_gemm(&B[ifm][oi],&A[ofm][ifm],&C[ofm][oi]);

							// //Unpack
							for (oi = 0; oi < ofw; ++oi) /*j loop */ {
								ii = oi * stride_w;
								for (ofm = t_ofm; ofm < min(nOfm, t_ofm + T_ofm); ofm++) /*i loop */ {
									output[img][ofm][oj][oi] = C[ofm - t_ofm][oi - 0];
									// C[0][0], C[0][1] ... = output[img][ofm][oj][oi], output[img][ofm+1][oj][oi+1]
								}
							}
						}
					}
				}
			}
		}
	}
}


#define ZERO(x) fabs(x - 0.0) < 0.0001

void padded_conv_fp_libxsmm_core502_gemm(int nImg, int nIfm, int nOfm, int ifhp, int ifwp, int ofhp, int ofwp, int ifh, int ifw,
	int ofh, int ofw, int pad_h, int pad_w, int pad_h_in, int pad_w_in, int pad_h_out,
	int pad_w_out, int kh, int kw, int stride_h, int stride_w,
	const float input[nImg][nIfm][ifhp + 2 * pad_h][ifwp + 2 * pad_w], float output[nImg][nOfm][ofhp][ofwp], const float filter[nOfm][nIfm][kh][kw]
/*,int iters*/)
{
int ldx;
ldx = stride_w * GEMM_BLOCK;
int* ldx_ptr = NULL;
if (stride_w > 1) {
	ldx_ptr = &ldx;
}

libxsmm_smmfunction fwd_gemm2;
fwd_gemm2 = libxsmm_smmdispatch(T_ofm, ofwp, T_ifm, NULL, ldx_ptr, NULL, NULL, NULL, NULL, NULL);
/* loop counters */
int img, ofm, ifm, oj, oi, ij, ii, kj, ki, t_ofm, t_ifm;
#pragma omp parallel for private(ofm, ifm,t_ofm, t_ifm,oj, ij, oi, ii, kj, ki)

for (img = 0; img < nImg; ++img) {
	// float C[T_ofm][ofw] -> C[ofw][T_ofm];
	// float B[T_ifm][ofw] -> B[ofw][T_ifm];
	// float A[T_ofm][T_ifm] -> A[T_ifm][T_ofm];
	// Column major: A[T_ifm][T_ofm] * B[ofw][T_ifm] = C[ofw][T_ofm]
	// Equivalent to row major: A[T_ofm][T_ifm] * B[T_ifm][ofw] = C[T_ofm][ofw]

	// libxsmm_smmdispatch(GEMM_BLOCK, ofwp, GEMM_BLOCK, NULL, ldx_ptr, NULL, NULL, NULL, NULL, NULL);
	// M = GEMM_BLOCK, N = ofwp, K = GEMM_BLOCK
	// Row major: A[M][K] * B[K][N] = C[M][N]
	// Column major: 
	// LIBXSMM_API libxsmm_smmfunction libxsmm_smmdispatch(libxsmm_blasint m, libxsmm_blasint n, libxsmm_blasint k,
	// const libxsmm_blasint* lda, const libxsmm_blasint* ldb, const libxsmm_blasint* ldc,

	float(*C)[T_ofm] =
		(float*)libxsmm_aligned_malloc(T_ofm*ofw * sizeof(float), 2097152);
	float(*B)[T_ifm] =
		(float*)libxsmm_aligned_malloc(T_ifm*ofw * sizeof(float), 2097152);
	float(*A)[T_ofm] =
		(float*)libxsmm_aligned_malloc(T_ofm*T_ifm * sizeof(float), 2097152);
	for (t_ofm = 0; t_ofm < nOfm; t_ofm += T_ofm) {
		for (t_ifm = 0; t_ifm < nIfm; t_ifm += T_ifm) {
			for (oj = 0; oj < ofh; ++oj) {
				ij = oj * stride_h;
				for (kj = 0; kj < kh; ++kj) {
					for (ki = 0; ki < kw; ++ki) {

						// Pack 
						// Packing code for C
						#pragma ivdep
						for (oi = 0; oi < ofw; ++oi) /*j loop */ {
							ii = oi * stride_w;
							for (ofm = t_ofm; ofm < min(nOfm, t_ofm + T_ofm); ofm++) /*i loop */ {
								C[oi - 0][ofm - t_ofm] = output[img][ofm][oj][oi];
								// C_prime[oi - 0][ofm - t_ofm] = C[oi - 0][ofm - t_ofm];
								// C[0][0], C[0][1] ... = output[img][ofm][oj][oi], output[img][ofm+1][oj][oi+1]
							}
						}

						//Packing code for B
						#pragma ivdep
						for (oi = 0; oi < ofw; ++oi) /*j loop */ {
							ii = oi * stride_w;
							for (ifm = t_ifm; ifm < min(nIfm, t_ifm + T_ifm); ifm++) /*i loop */ {
								B[ii - 0][ifm - t_ifm] = input[img][ifm][ij + kj][ii + ki];
								// C[0][0], C[0][1] ... = output[img][ofm][oj][oi], output[img][ofm+1][oj][oi+1]
							}
						}

						//Packing code for A
						#pragma ivdep
						for (ifm = t_ifm; ifm < min(nIfm, t_ifm + T_ifm); ifm++) /*i loop */ {
							for (ofm = t_ofm; ofm < min(nOfm, t_ofm + T_ofm); ofm++) /*j loop */ {
								A[ifm - t_ifm][ofm - t_ofm] = filter[ofm][ifm][kj][ki];
								// C[0][0], C[0][1] ... = output[img][ofm][oj][oi], output[img][ofm+1][oj][oi+1]
							}
						}

						/*
						printf("ofw = %d, T_ofm = %d\n", ofw, T_ofm);
						for (int i = 0; i < ofw; i++) {
							for (int j = 0; j < T_ofm; j++) {
								if (!ZERO(C[i][j]) || !ZERO(C_prime[i][j])) {
									printf("C[%d][%d] = %f, C_prime[%d][%d] = %f\n",
										i, j, C[i][j], i, j, C_prime[i][j]);
								}
							}
						}
						*/

						// Gemm
						// for (oi = 0; oi < ofw; ++oi) /*j loop */ {
						// 	ii = oi * stride_w;
						// 	for (ofm = t_ofm; ofm < t_ofm + T_ofm /* min(nOfm, t_ofm + T_ofm) */;
						// 		ofm++) /*i loop */ {
						// 		for (ifm = t_ifm; ifm < t_ifm + T_ifm /* min(nIfm, t_ifm + T_ifm) */;
						// 			ifm++) /* k loop */ {
						// 			// output[img][ofm][oj][oi] +=
						// 			// filter[ofm][ifm][kj][ki] /* filter - A */ * input[img][ifm][ij + kj][ii + ki] /*input - B */;
						// 			C[oi][ofm - t_ofm] += A[ifm - t_ifm][ofm - t_ofm] * B[oi][ifm - t_ifm];

						// 		}
						// 	}
						// }

						fwd_gemm2(&A[0][0], &B[0][0], &C[0][0]);

						// C[ofw][T_ofm]

						/*
						printf("ofw = %d, T_ofm = %d\n", ofw, T_ofm);
						for (int i = 0; i < ofw; i++) {
							for (int j = 0; j < T_ofm; j++) {
								if (!ZERO(C[i][j]) || !ZERO(C_prime[i][j])) {
									printf("C[%d][%d] = %f, C_prime[%d][%d] = %f\n",
										i, j, C[i][j], i, j, C_prime[i][j]);
								}
							}
						}
						exit(1);
						*/

						//Unpack
						#pragma ivdep
						for (oi = 0; oi < ofw; ++oi) /*j loop */ {
							ii = oi * stride_w;
							for (ofm = t_ofm; ofm < min(nOfm, t_ofm + T_ofm); ofm++) /*i loop */ {
								output[img][ofm][oj][oi] = C[oi - 0][ofm - t_ofm];
								// C[0][0], C[0][1] ... = output[img][ofm][oj][oi], output[img][ofm+1][oj][oi+1]
							}
						}
					}
				}
			}
		}
	}

	libxsmm_free(C);
	libxsmm_free(A);
	libxsmm_free(B);
}
}