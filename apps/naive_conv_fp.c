void naive_conv_fp_fn(
	int nImg, int nIfm, int nOfm, int ifhp, int ifwp, int ofhp, int ofwp, int ifh, int ifw,
	int ofh, int ofw, int pad_h, int pad_w, int pad_h_in, int pad_w_in, int pad_h_out,
	int pad_w_out, int kh, int kw, int stride_h, int stride_w,
	const float input[nImg][nIfm][ifhp][ifwp], float output[nImg][nOfm][ofhp][ofwp], const float filter[nOfm][nIfm][kh][kw])
{
	/* loop counters */
	int img, ofm, ifm, oj, oi, ij, ii, kj, ki;

	/*
	float input[nImg][nIfm][ifhp][ifwp];
	float output[nImg][nOfm][ofhp][ofwp];
	float filter[nOfm][nIfm][kh][kw];
	*/

#pragma omp parallel for private(ofm, ifm, oj, ij, oi, ii, kj, ki)
	for (img = 0; img < nImg; ++img) {
		for (ofm = 0; ofm < nOfm; ++ofm) {
			for (ifm = 0; ifm < nIfm; ++ifm) {
				for (oj = 0; oj < ofh; ++oj) {
					ij = oj * stride_h - pad_h;
					for (oi = 0; oi < ofw; ++oi) {
						ii = oi * stride_w - pad_w;
						for (kj = 0; kj < kh; ++kj) {
							if (ij + kj < 0 || ij + kj >= ifh) continue;
							for (ki = 0; ki < kw; ++ki) {
								if (ii + ki < 0 || ii + ki >= ifw) continue;
								output[img][ofm][oj][oi] += input[img][ifm][ij + kj][ii + ki]
									* filter[ofm][ifm][kj][ki];
							}
						}
					}
				}
			}
		}
	}
}