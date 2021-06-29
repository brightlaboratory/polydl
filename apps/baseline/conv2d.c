#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

void padded_conv_fp_fn(
		int nImg, int nIfm, int nOfm, int ifhp, int ifwp, int ofhp, int ofwp, int ifh, int ifw,
		int ofh, int ofw, int pad_h, int pad_w, int pad_h_in, int pad_w_in, int pad_h_out,
		int pad_w_out, int kh, int kw, int stride_h, int stride_w,
		const float input[nImg][nIfm][ifhp+2*pad_h][ifwp+2*pad_w], float output[nImg][nOfm][ofhp][ofwp], const float filter[nOfm][nIfm][kh][kw])
{
	/* loop counters */
	int img, ofm, ifm, oj, oi, ij, ii, kj, ki;

#pragma omp parallel for private(ofm, ifm, oj, ij, oi, ii, kj, ki)
	for (img = 0; img < nImg; ++img) {
		for (ofm = 0; ofm < nOfm; ++ofm) {
			for (ifm = 0; ifm < nIfm; ++ifm) {
				for (oj = 0; oj < ofh; ++oj) {
					ij = oj * stride_h;
					for (oi = 0; oi < ofw; ++oi) {
						ii = oi * stride_w;
						for (kj = 0; kj < kh; ++kj) {
							for (ki = 0; ki < kw; ++ki) {
								output[img][ofm][oj][oi] += 
									input[img][ifm][ij + kj][ii + ki]
									* filter[ofm][ifm][kj][ki];
							}
						}
					}
				}
			}
		}
	}
}

double rtclock() {
	struct timezone Tzp;
	struct timeval Tp;
	int stat;
	stat = gettimeofday(&Tp, &Tzp);
	if (stat != 0)
		fprintf(stderr, "Error return from gettimeofday: %d", stat);
	return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}

void zero_buf(float* buf, long size) {
	int i;
	for (i = 0; i < size; ++i) {
		buf[i] = 0.0f;
	}
}

void init_buf(float* buf, long size, int initPos, int initOne)
{
	int i;
	zero_buf(buf, size);
	for (i = 0; i < size; ++i) {
		buf[i] = (float)((initOne != 0) ? 1.0 : ((initPos != 0) ? drand48() : (0.05 - drand48() / 10.0)));
	}
}


void copy_NCHW_to_PADDED_NCHW(int N, int C, int H, int W, int pad_h,
		int pad_w,
		const float input[N][C][H][W],
		float pad_input[N][C][H + 2 * pad_h][W + 2 * pad_w])
{
	int n, h, w, c;

	for (n = 0; n < N; n++) {
		for (c = 0; c < C; c++) {
			for (h = 0; h < H; h++) {
				for (w = 0; w < W; w++) {
					pad_input[n][c][h + pad_h][w + pad_w]
						= input[n][c][h][w];
				}
			}
		}
	}
}


void set_zeropad_nchw(int N, int C, int H, int W, int pad_h, int pad_w, float input[N][C][H][W])
{
	int n, h, w, c;

	for (n = 0; n < N; n++) {
		for (c = 0; c < C; c++) {
			for (h = 0; h < H; h++) {
				for (w = 0; w < W; w++) {
					if (h < pad_h || h >= H - pad_h || w < pad_w || w >= W - pad_w)
						input[n][c][h][w] = 0.0;
				}
			}
		}
	}
}


int main(int argc, char **argv) {
	int ifhp, ifwp, ofhp, ofwp, ofh, ofw;
	int stride_h, stride_w, pad_h, pad_w, pad_h_in, pad_w_in, pad_h_out, pad_w_out;


	/* some parameters we can overwrite via cli,
	   default is some inner layer of overfeat */
	int iters = 10;         /* repetitions of benchmark */
	int ifw = 14;           /* input width, "W" */
	int ifh = 18;           /* input height, "H" */
	int nImg = 32;          /* mini-batch size, "N" */
	int nIfm = 256;         /* number of input feature maps, "C" */
	int nOfm = 512;         /* number of output feature maps, "K" */
	int kh = 3;             /* filter height, "R" */
	int kw = 3;             /* filter width, "S" */
	int pad = 2;            /* padding in output */
	int stride = 1;         /* stride when accessing inputs */

	pad_w = pad;
	pad_h = pad;

	double l_total = 0.0;
	double flops = 0.0;

	/* reading new values from cli */
	int i = 1;
	if (argc > i) iters = atoi(argv[i++]);
	if (argc > i) ifw = atoi(argv[i++]);
	if (argc > i) ifh = atoi(argv[i++]);
	if (argc > i) nIfm = atoi(argv[i++]);
	if (argc > i) nOfm = atoi(argv[i++]);
	if (argc > i) kw = atoi(argv[i++]);
	if (argc > i) kh = atoi(argv[i++]);
	if (argc > i) pad_w = atoi(argv[i++]);
	if (argc > i) pad_h = atoi(argv[i++]);
	if (argc > i) stride = atoi(argv[i++]);
	if (argc > i) nImg = atoi(argv[i++]);


	/* apply stride in both dimensions */
	stride_w = stride;
	stride_h = stride;

	pad_h_in = 0;
	pad_w_in = 0;
	pad_h_out = 0;
	pad_w_out = 0;

	/* deriving some values image size */
	ofh = (ifh + 2 * pad_h - kh) / stride_h + 1;
	ofw = (ifw + 2 * pad_w - kw) / stride_w + 1;
	ifhp = ifh + 2 * pad_h_in;
	ifwp = ifw + 2 * pad_w_in;
	ofhp = ofh + 2 * pad_h_out;
	ofwp = ofw + 2 * pad_w_out;


	/* print some summary */
	printf("##########################################\n");
	printf("#                Setting Up              #\n");
	printf("##########################################\n");
	printf("PARAMS: W:%d  H:%d  N:%d  C:%d  K:%d  R:%d  S:%d  P:%d  Q:%d  STRIDE:%d\n", ifw, ifh, nImg, nIfm, nOfm, kw, kh, ofh, ofw, stride);
	printf("PARAMS: ITERS:%d", iters);
	printf(" InImg %dx%d Padded (%dx%d)\n", ifh, ifw, ifhp, ifwp);
	printf("OutImg %dx%d Padded (%dx%d)\n", ofh, ofw, ofhp, ofwp);
	printf("SIZE Input  (MB): %10.2f MiB\n", (double)(nImg*nIfm*ifhp*ifwp * sizeof(float)) / (1024.0*1024.0));
	printf("SIZE Output (MB): %10.2f MiB\n", (double)(nImg*nOfm*ofhp*ofwp * sizeof(float)) / (1024.0*1024.0));
	printf("SIZE Input   (1): %10.2f MiB\n", (double)(1 * nIfm*ifhp*ifwp * sizeof(float)) / (1024.0*1024.0));
	printf("SIZE Output  (1): %10.2f MiB\n", (double)(1 * nOfm*ofhp*ofwp * sizeof(float)) / (1024.0*1024.0));
	printf("SIZE Weight     : %10.2f MiB\n", (double)(nIfm*nOfm*kw*kh * sizeof(float)) / (1024.0*1024.0));


	printf("Allocating data\n");
	/* allocate data */
	float(*input)[nIfm][ifhp][ifwp] =
		(float*)malloc(nImg*nIfm*ifhp*ifwp * sizeof(float));

	float(*output)[nOfm][ofhp][ofwp] =
		(float*)malloc(nImg*nOfm*ofhp*ofwp * sizeof(float));

	float(*filter)[nIfm][kh][kw] =
		(float*)malloc(nOfm*nIfm*kh*kw * sizeof(float));

	printf("Initializing data\n");
	/* initialize data */
	srand48(1);
	init_buf(&input[0][0][0][0], nImg*nIfm*ifhp*ifwp, 0, 0);
	// set_zeropad_nchw(nImg, nIfm, ifhp, ifwp, pad_h, pad_w, input);
	init_buf(&filter[0][0][0][0], nOfm*nIfm*kh*kw, 0, 0);
	zero_buf(&output[0][0][0][0], nImg*nOfm*ofhp*ofwp);

	float(*pad_input)[nIfm][ifhp + 2 * pad_h][ifwp + 2 * pad_w]
		= (float*)malloc(nImg*nIfm*(ifhp + 2 * pad_h)*(ifwp + 2 * pad_w) * sizeof(float));
	zero_buf(&pad_input[0][0][0][0],
			nImg*nIfm*(ifhp + 2 * pad_h)*(ifwp + 2 * pad_w));

	copy_NCHW_to_PADDED_NCHW(nImg, nIfm, ifhp, ifwp, pad_h, pad_w,
			input, pad_input);

	flops = (double)nImg * (double)nIfm * (double)nOfm * (double)ofh * (double)ofw * (double)(2 * kh * kw) * (double)iters;

	double t_start, t_end;
	double t_sum = 0;

	printf("Running convolutions for %d iterations\n", iters);
	t_start = rtclock();
	for (i = 0; i < iters; i++) {
		padded_conv_fp_fn(nImg, nIfm, nOfm, ifhp, ifwp, ofhp, ofwp, ifh, ifw,
				ofh, ofw, pad_h, pad_w, pad_h_in, pad_w_in, pad_h_out,
				pad_w_out, kh, kw, stride_h, stride_w,
				pad_input, output, filter);
	}

	t_end = rtclock();
	t_sum += (t_end - t_start);

	printf("Elapsed time of padded_conv_f = %f seconds\n", t_sum);
	printf("GFLOPS =%.5g\n", (flops*1e-9) / t_sum);

	printf("Outputs = %f %f %f\n", output[0][0][0][0], 
			output[nImg/2][nOfm/2][ofhp/2][ofwp/2], output[nImg-1][nOfm-1][ofhp-1][ofwp-1]);

	free(input);
	free(output);
	free(filter);
	free(pad_input);

	return 0;
}

