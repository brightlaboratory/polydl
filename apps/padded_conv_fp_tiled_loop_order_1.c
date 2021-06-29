#ifndef STRIDE_H
#define STRIDE_H 1
#endif // !STRIDE_H

#ifndef STRIDE_W
#define STRIDE_W 1
#endif // !STRIDE_W

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

#ifndef T_ofm_tile
#define T_ofm_tile 4
#endif // !T_ofm_tile

#ifndef T_ifm_tile
#define T_ifm_tile 1
#endif // !T_ifm_tile

#ifndef T_oj
#define T_oj 4
#endif // !T_oj

#ifndef T_oi
#define T_oi 28
#endif // !T_oi

#ifndef GEMM_BLOCK
#define GEMM_BLOCK 64
#endif // !GEMM_BLOCK

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

static inline void padded_conv_fp_tiled_loop_order_1_fn(int nImg, int nIfm, int nOfm, int ifhp, int ifwp, int ofhp, int ofwp, int ifh, int ifw,
	int ofh, int ofw, int pad_h, int pad_w, int pad_h_in, int pad_w_in, int pad_h_out,
	int pad_w_out, int kh, int kw, int stride_h, int stride_w,
	const float pad_gemm_input[nImg][nIfm / GEMM_BLOCK][ifhp + 2 * pad_h][ifwp + 2 * pad_w][GEMM_BLOCK], float output[nImg][nOfm / GEMM_BLOCK][ofhp][ofwp][GEMM_BLOCK], const float filter[nOfm / GEMM_BLOCK][nIfm / GEMM_BLOCK][kh][kw][GEMM_BLOCK][GEMM_BLOCK], int iters)
{
	// printf("LIBXMM version\n");
	/*
	FWD params...
Fwd_ofw_rb = 28
Fwd_ofh_rb = 1
Pack input = 0
Block oj = 4
Loop order = 1
Blocksifm_blocking = 1
Block fwd ofm = 4
Block fwd ifm = 1
Avoid rim fmas = 0
Ofm parallelization = 0
Shuffle filter accesses = 0
Avoid acc load = 1
Fwd GEMM flags = 640
	*/

	/* loop counters */
	int img, ofm_tile, ofm, ifm_tile, ifm, oj, oi, ij, ii, kj, ki, i;
	int t_ofm_tile, t_ifm_tile, t_oj, t_oi;

#pragma scop
#pragma omp parallel for private(img, t_ofm_tile, t_oj, oj, t_oi, ofm_tile, t_ifm_tile, ifm_tile, kj, ki, ii, ij)
	for (img = 0; img < nImg; ++img) {
		for (t_ofm_tile = 0; t_ofm_tile < nOfm / GEMM_BLOCK; t_ofm_tile += T_ofm_tile) {
			for (t_oj = 0; t_oj < ofh; t_oj += T_oj) {
				for (oj = t_oj; oj < min(ofh, t_oj + T_oj); ++oj) {
					ij = oj * STRIDE_H;
					for (t_oi = 0; t_oi < ofw; t_oi += T_oi) {
						for (ofm_tile = t_ofm_tile; ofm_tile < min(nOfm / GEMM_BLOCK, t_ofm_tile + T_ofm_tile); ++ofm_tile) {
							for (t_ifm_tile = 0; t_ifm_tile < nIfm / GEMM_BLOCK; t_ifm_tile += T_ifm_tile) {
								for (ifm_tile = t_ifm_tile; ifm_tile < min(nIfm / GEMM_BLOCK, t_ifm_tile + T_ifm_tile); ++ifm_tile) {
									for (kj = 0; kj < kh; ++kj) {
										for (ki = 0; ki < kw; ++ki) {


											// GEMM
											// min(ofw, t_oi + T_oi) is simplified to t_oi + T_oi because T_oi divides ofw.
											for (oi = t_oi; oi < t_oi + T_oi; ++oi) {
												ii = oi * STRIDE_W;
												for (ofm = 0; ofm < GEMM_BLOCK; ++ofm) {
													for (ifm = 0; ifm < GEMM_BLOCK; ++ifm) {
														output[img][ofm_tile][oj][oi][ofm] +=
															filter[ofm_tile][ifm_tile][kj][ki][ifm][ofm] * pad_gemm_input[img][ifm_tile][ij + kj][ii + ki][ifm];
													}
												}
											}


											/*												fwd_gemm(&filter[ofm_tile][ifm_tile][kj][ki][0][0],
																								&pad_gemm_input[img][ifm_tile][ij + kj][t_oi + ki][0],
																								&output[img][ofm_tile][oj][t_oi][0]);
											*/

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
#pragma endscop
}

inline void padded_conv_fp_tiled_loop_order_1_gemm(int nImg, int nIfm, int nOfm, int ifhp, int ifwp, int ofhp, int ofwp, int ifh, int ifw,
	int ofh, int ofw, int pad_h, int pad_w, int pad_h_in, int pad_w_in, int pad_h_out,
	int pad_w_out, int kh, int kw, int stride_h, int stride_w,
	const float pad_gemm_input[nImg][nIfm / GEMM_BLOCK][ifhp + 2 * pad_h][ifwp + 2 * pad_w][GEMM_BLOCK], float output[nImg][nOfm / GEMM_BLOCK][ofhp][ofwp][GEMM_BLOCK], const float filter[nOfm / GEMM_BLOCK][nIfm / GEMM_BLOCK][kh][kw][GEMM_BLOCK][GEMM_BLOCK], int iters)
{
	// printf("LIBXMM version\n");
	/*
	FWD params...
Fwd_ofw_rb = 28
Fwd_ofh_rb = 1
Pack input = 0
Block oj = 4
Loop order = 1
Blocksifm_blocking = 1
Block fwd ofm = 4
Block fwd ifm = 1
Avoid rim fmas = 0
Ofm parallelization = 0
Shuffle filter accesses = 0
Avoid acc load = 1
Fwd GEMM flags = 640
	*/

	/* loop counters */
	int img, ofm_tile, ofm, ifm_tile, ifm, oj, oi, ij, ii, kj, ki, i;
	int t_ofm_tile, t_ifm_tile, t_oj, t_oi;


#pragma omp parallel for private(img, t_ofm_tile, t_oj, oj, t_oi, ofm_tile, t_ifm_tile, ifm_tile, kj, ki, ii, ij)
	for (img = 0; img < nImg; ++img) {
		// zero_buf(&output[img][0][0][0][0], nOfm*ofhp*ofwp);
		for (t_ofm_tile = 0; t_ofm_tile < nOfm / GEMM_BLOCK; t_ofm_tile += T_ofm_tile) {
			for (t_oj = 0; t_oj < ofh; t_oj += T_oj) {
				for (oj = t_oj; oj < min(ofh, t_oj + T_oj); ++oj) {
					ij = oj * stride_h;
					for (t_oi = 0; t_oi < ofw; t_oi += T_oi) {
						for (ofm_tile = t_ofm_tile; ofm_tile < min(nOfm / GEMM_BLOCK, t_ofm_tile + T_ofm_tile); ++ofm_tile) {
							for (t_ifm_tile = 0; t_ifm_tile < nIfm / GEMM_BLOCK; t_ifm_tile += T_ifm_tile) {
								for (ifm_tile = t_ifm_tile; ifm_tile < min(nIfm / GEMM_BLOCK, t_ifm_tile + T_ifm_tile); ++ifm_tile) {
									for (kj = 0; kj < kh; ++kj) {
										for (ki = 0; ki < kw; ++ki) {

											/*
											// GEMM
											// min(ofw, t_oi + T_oi) is simplified to t_oi + T_oi because T_oi divides ofw.
											for (oi = t_oi; oi < t_oi + T_oi; ++oi) {
												ii = oi * stride_w;
												for (ofm = 0; ofm < GEMM_BLOCK; ++ofm) {
													for (ifm = 0; ifm < GEMM_BLOCK; ++ifm) {
														output[img][ofm_tile][oj][oi][ofm] +=
															filter[ofm_tile][ifm_tile][kj][ki][ifm][ofm] * pad_gemm_input[img][ifm_tile][ij + kj][ii + ki][ifm];
													}
												}
											}
											*/

											fwd_gemm(&filter[ofm_tile][ifm_tile][kj][ki][0][0],
												&pad_gemm_input[img][ifm_tile][ij + kj][t_oi + ki][0],
												&output[img][ofm_tile][oj][t_oi][0]);


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


static inline void padded_conv_fp_tiled_loop_order_1_vanilla_gemm(int nImg, int nIfm, int nOfm, int ifhp, int ifwp, int ofhp, int ofwp, int ifh, int ifw,
	int ofh, int ofw, int pad_h, int pad_w, int pad_h_in, int pad_w_in, int pad_h_out,
	int pad_w_out, int kh, int kw, int stride_h, int stride_w,
	const float pad_gemm_input[nImg][nIfm / GEMM_BLOCK][ifhp + 2 * pad_h][ifwp + 2 * pad_w][GEMM_BLOCK], float output[nImg][nOfm / GEMM_BLOCK][ofhp][ofwp][GEMM_BLOCK], const float filter[nOfm / GEMM_BLOCK][nIfm / GEMM_BLOCK][kh][kw][GEMM_BLOCK][GEMM_BLOCK], int iters)
{
	// printf("LIBXMM version\n");
	/*
	FWD params...
Fwd_ofw_rb = 28
Fwd_ofh_rb = 1
Pack input = 0
Block oj = 4
Loop order = 1
Blocksifm_blocking = 1
Block fwd ofm = 4
Block fwd ifm = 1
Avoid rim fmas = 0
Ofm parallelization = 0
Shuffle filter accesses = 0
Avoid acc load = 1
Fwd GEMM flags = 640
	*/

	/* loop counters */
	int img, ofm_tile, ofm, ifm_tile, ifm, oj, oi, ij, ii, kj, ki, i;
	int t_ofm_tile, t_ifm_tile, t_oj, t_oi;

#pragma omp parallel for private(img, t_ofm_tile, t_oj, oj, t_oi, ofm_tile, t_ifm_tile, ifm_tile, kj, ki, ii, ij)
	for (img = 0; img < nImg; ++img) {
		for (t_ofm_tile = 0; t_ofm_tile < nOfm / GEMM_BLOCK; t_ofm_tile += T_ofm_tile) {
			for (t_oj = 0; t_oj < ofh; t_oj += T_oj) {
				for (oj = t_oj; oj < min(ofh, t_oj + T_oj); ++oj) {
					ij = oj * STRIDE_H;
					for (t_oi = 0; t_oi < ofw; t_oi += T_oi) {
						for (ofm_tile = t_ofm_tile; ofm_tile < min(nOfm / GEMM_BLOCK, t_ofm_tile + T_ofm_tile); ++ofm_tile) {
							for (t_ifm_tile = 0; t_ifm_tile < nIfm / GEMM_BLOCK; t_ifm_tile += T_ifm_tile) {
								for (ifm_tile = t_ifm_tile; ifm_tile < min(nIfm / GEMM_BLOCK, t_ifm_tile + T_ifm_tile); ++ifm_tile) {
									for (kj = 0; kj < kh; ++kj) {
										for (ki = 0; ki < kw; ++ki) {

											// GEMM
											// min(ofw, t_oi + T_oi) is simplified to t_oi + T_oi because T_oi divides ofw.
											gemm(T_oi, GEMM_BLOCK, GEMM_BLOCK,
												&pad_gemm_input[img][ifm_tile][ij + kj][t_oi + ki][0],
												GEMM_BLOCK * stride_w,
												&filter[ofm_tile][ifm_tile][kj][ki][0][0], GEMM_BLOCK,
												&output[img][ofm_tile][oj][t_oi][0], GEMM_BLOCK);

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
