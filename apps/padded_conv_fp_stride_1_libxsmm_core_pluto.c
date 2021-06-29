#include <omp.h>
#include <math.h>
#define ceild(n,d)  ceil(((double)(n))/((double)(d)))
#define floord(n,d) floor(((double)(n))/((double)(d)))
#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))

#ifndef GEMM_BLOCK
#define GEMM_BLOCK 64
#endif // !GEMM_BLOCK

void padded_conv_fp_stride_1_libxsmm_core_pluto(int nImg, int nIfm, int nOfm, int ifhp, int ifwp, int ofhp, int ofwp, int ifh, int ifw,
	int ofh, int ofw, int pad_h, int pad_w, int pad_h_in, int pad_w_in, int pad_h_out,
	int pad_w_out, int kh, int kw, int stride_h, int stride_w,
	const float pad_gemm_input[nImg][nIfm / GEMM_BLOCK][ifhp + 2 * pad_h][ifwp + 2 * pad_w][GEMM_BLOCK], float output[nImg][nOfm / GEMM_BLOCK][ofhp][ofwp][GEMM_BLOCK], const float filter[nOfm / GEMM_BLOCK][nIfm / GEMM_BLOCK][kh][kw][GEMM_BLOCK][GEMM_BLOCK], int iters)
{
	/* loop counters */
	int img, ofm_tile, ofm, ifm_tile, ifm, oj, oi, ij, ii, kj, ki, i;

	int t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
	int lb, ub, lbp, ubp, lb2, ub2;
	int iter_loop;
	register int lbv, ubv;
	if ((kh >= 1) && (kh <= -ofh + 2147483649) && (kw >= 1) && (kw <= -ofw + 2147483649) && (nIfm >= 64) && (nImg >= 1) && (nOfm >= 64) && (ofh >= 1) && (ofw >= 1)) {
		lbp = 0;
		ubp = floord(nImg - 1, 1);
		for (iter_loop = 0; iter_loop < iters; iter_loop++) {
#pragma omp parallel for private(lbv,ubv,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15)
			for (t1 = lbp; t1 <= ubp; t1++) {
				for (t2 = 0; t2 <= floord(nOfm - 64, 2048); t2++) {
					for (t3 = 0; t3 <= floord(ofh - 1, 32); t3++) {
						for (t4 = 0; t4 <= floord(ofw - 1, 32); t4++) {
							for (t5 = 0; t5 <= 1; t5++) {
								for (t6 = 0; t6 <= floord(nIfm - 64, 2048); t6++) {
									for (t7 = 1 * t1; t7 <= min(nImg - 1, 1 * t1 + 0); t7++) {
										for (t8 = 32 * t2; t8 <= min(floord(nOfm - 64, 64), 32 * t2 + 31); t8++) {
											for (t9 = 32 * t3; t9 <= min(ofh - 1, 32 * t3 + 31); t9++) {
												for (t10 = 32 * t4; t10 <= min(ofw - 1, 32 * t4 + 31); t10++) {
													for (t11 = 32 * t5; t11 <= 32 * t5 + 31; t11++) {
														for (t12 = 32 * t6; t12 <= min(floord(nIfm - 64, 64), 32 * t6 + 31); t12++) {
															for (t13 = 0; t13 <= kh - 1; t13++) {
																for (t14 = 0; t14 <= kw - 1; t14++) {
																	for (t15 = 0; t15 <= 63; t15++) {
																		output[t7][t8][t9][t10][t11] += (filter[t8][t12][t13][t14][t15][t11] * pad_gemm_input[t7][t12][t9 + t13][t14 + t10][t15]);;
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
