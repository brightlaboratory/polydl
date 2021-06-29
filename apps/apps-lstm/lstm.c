#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <libxsmm.h>



void matrix_add(int size, float *a, float *b, float *c)
{
  int i;
    #if defined(_OPENMP)
    # pragma omp parallel for private(i)
    #endif
  for (i = 0; i < size; i++) {
    c[i] = a[i] + b[i];
  }
}

void matrix_eltwise_mult(int size, float *a, float *b, float *c)
{
  int i;
    #if defined(_OPENMP)
    # pragma omp parallel for private(i)
    #endif
  for (i = 0; i < size; i++) {
    c[i] = a[i] * b[i];
  }
}

void matrix_sigmoid(int size, float *src, float *dst)
{
  int i;
    #if defined(_OPENMP)
    # pragma omp parallel for private(i)
    #endif
  for (i = 0; i < size; i++) {
    const float exp_value = (float)exp((double) -src[i]);
    dst[i] = 1.0f / (1.0f + exp_value);
  }
}

void matrix_tanh(int size, float *src, float *dst)
{
  int i;
    #if defined(_OPENMP)
    # pragma omp parallel for private(i)
    #endif
  for (i = 0; i < size; i++) {
    dst[i] = (float)tanh((double)src[i]);
  }
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

void print(float* buf, long size) {
	int i;
	for (i = 0; i < size; ++i) {
		printf("%f , ", buf[i]);
	}
}

 /*Send output K instead of input C while evaluating for r*/   
void multiply(int N,int C, int K,float mat1[K][C], float mat2[C][N], float res[K][N])
{ 
    int i, j, k; 
    for (i = 0; i < K; i++) 
    { 
        for (j = 0; j < N; j++) 
        { 
            res[i][j] = 0; 
            for (k = 0; k < C; k++) 
                res[i][j] += mat1[i][k] *  
                             mat2[k][j]; 
        } 
    } 
} 

void lstm_fwd(int N,int C, int K,
             const float wfgold[K][C], const float wigold[K][C], const float wogold[K][C], const float wcgold[K][C],
             const float rfgold[K][K], const float rigold[K][K], const float rogold[K][K], const float rcgold[K][K],
             const float wgold[C][N],float rgold[K][N],
             const float bf[K][N], const float bi[K][N], const float bo[K][N], const float bc[K][N],
             float cpgold[K][N])
{

  float fgold[K][N], frgold[K][N];
  float igold[K][N], irgold[K][N];
  float ogold[K][N], orgold[K][N];
  float cgold[K][N], crgold[K][N];


  /* Calculating forget state - ft */

    
    multiply(N,C,K,wfgold,wgold,fgold);
    
    multiply(N,K,K,rfgold,rgold,frgold);

    matrix_add(K*N, &fgold[0][0],&frgold[0][0],&fgold[0][0]);
    matrix_add(K*N, &fgold[0][0],&bf[0][0],&fgold[0][0]);
    matrix_sigmoid(K*N, &fgold[0][0] , &fgold[0][0]);

  /* Calculating Input state - it */

    
    multiply(N,C,K,wigold,wgold,igold);
    
    multiply(N,K,K,rfgold,rgold,irgold);

    matrix_add(K*N, &igold[0][0],&irgold[0][0],&igold[0][0]);
    matrix_add(K*N, &igold[0][0],&bi[0][0],&igold[0][0]);
    matrix_sigmoid(K*N, &igold[0][0] , &igold[0][0]);

  /* Calculating output state - ot */

    
    multiply(N,C,K,wogold,wgold,ogold);
    
    multiply(N,K,K,rogold,rgold,orgold);

    matrix_add(K*N, &ogold[0][0],&orgold[0][0],&ogold[0][0]);
    matrix_add(K*N, &ogold[0][0],&bo[0][0],&ogold[0][0]);
    matrix_sigmoid(K*N, &ogold[0][0] , &ogold[0][0]);

  /* Calculating cell state - ct */

    
    multiply(N,C,K,wcgold,wgold,cgold);
    
    multiply(N,K,K,rcgold,rgold,crgold);

    matrix_add(K*N, &cgold[0][0],&crgold[0][0],&cgold[0][0]);
    matrix_add(K*N, &cgold[0][0],&bc[0][0],&cgold[0][0]);
    matrix_tanh(K*N, &cgold[0][0] , &cgold[0][0]);

    matrix_eltwise_mult(K*N, &igold[0][0], &cgold[0][0], &igold[0][0]);
    matrix_eltwise_mult(K*N, &fgold[0][0], &cpgold[0][0], &fgold[0][0]);

    matrix_add(K*N, &fgold[0][0],&igold[0][0],&cpgold[0][0]);


  /* Calculating hidden state - ht 
    Here cpgold no contains the currenct cell value so that it can be reused for next iteration.
  */


    matrix_tanh(K*N, &cpgold[0][0] , &cgold[0][0]);
    matrix_eltwise_mult(K*N, &ogold[0][0], &cgold[0][0], &rgold[0][0]);


}



int main(int argc, char **argv)
{
  /* Initialized to Default Values */
  int N = 1;        /* Size of mini-batch */  
  int C = 100;      /* number of inputs */
  int K = 100;      /* number of outputs */
  int t = 2;       /* number of time stamps */

 	int i = 1;
	if (argc > i) N     = atoi(argv[i++]);
  if (argc > i) C     = atoi(argv[i++]);
  if (argc > i) K     = atoi(argv[i++]);
  if (argc > i) t     = atoi(argv[i++]);

  float(*wfgold)[K][C]=(float*)libxsmm_aligned_malloc(K*C* sizeof(float), 2097152);
  float(*wigold)[K][C]=(float*)libxsmm_aligned_malloc(K*C* sizeof(float), 2097152);
  float(*wogold)[K][C]=(float*)libxsmm_aligned_malloc(K*C* sizeof(float), 2097152);
  float(*wcgold)[K][C]=(float*)libxsmm_aligned_malloc(K*C* sizeof(float), 2097152);
  float(*rfgold)[K][K]=(float*)libxsmm_aligned_malloc(K*K* sizeof(float), 2097152);
  float(*rigold)[K][K]=(float*)libxsmm_aligned_malloc(K*K* sizeof(float), 2097152);
  float(*rogold)[K][K]=(float*)libxsmm_aligned_malloc(K*K* sizeof(float), 2097152);
  float(*rcgold)[K][K]=(float*)libxsmm_aligned_malloc(K*K* sizeof(float), 2097152);

  float(*wgold)[C][N]=(float*)libxsmm_aligned_malloc(C*N* sizeof(float), 2097152);
  float(*rgold)[K][N]=(float*)libxsmm_aligned_malloc(K*N* sizeof(float), 2097152);
  
  float(*cpgold)[K][N]=(float*)libxsmm_aligned_malloc(K*N* sizeof(float), 2097152);

  float(*bf)[K][N]=(float*)libxsmm_aligned_malloc(K*N* sizeof(float), 2097152);
  float(*bi)[K][N]=(float*)libxsmm_aligned_malloc(K*N* sizeof(float), 2097152);
  float(*bo)[K][N]=(float*)libxsmm_aligned_malloc(K*N* sizeof(float), 2097152);
  float(*bc)[K][N]=(float*)libxsmm_aligned_malloc(K*N* sizeof(float), 2097152);

/* allocate data */
	srand48(1);
	init_buf(&wfgold[0][0], K*C, 0, 0);
	init_buf(&wigold[0][0], K*C, 0, 0);
	init_buf(&wogold[0][0], K*C, 0, 0);
	init_buf(&wcgold[0][0], K*C, 0, 0);
	init_buf(&rfgold[0][0], K*K, 0, 0);
	init_buf(&rigold[0][0], K*K, 0, 0);
	init_buf(&rogold[0][0], K*K, 0, 0);
	init_buf(&rcgold[0][0], K*K, 0, 0);
	init_buf(&wgold[0][0], C*N, 0, 0);
	zero_buf(&rgold[0][0], K*N);
  zero_buf(&cpgold[0][0], K*N);

  init_buf(&bf[0][0], K*N, 0, 0);
  init_buf(&bi[0][0], K*N, 0, 0);
  init_buf(&bo[0][0], K*N, 0, 0);
  init_buf(&bc[0][0], K*N, 0, 0);

    // printf("\nPrinting values of wfgold \n");
    // print(&wfgold[0][0], K*C);


unsigned long long l_start, l_end;
	double l_total = 0.0;
	double flops = 0.0;

  l_start = libxsmm_timer_tick();

for(int j=0; j<t; ++j){
  lstm_fwd( N, C, K,
                    wfgold, wigold, wogold, wcgold,
                    rfgold, rigold, rogold, rcgold,
                    wgold, rgold, bf,bi,bo,bc,
                    cpgold);

  l_end = libxsmm_timer_tick();
  l_total = libxsmm_timer_duration(l_start, l_end);
  printf("Elapsed time of padded_conv_fp = %f seconds\n", l_total);

  /* Printing cpgold i.e (Cell State) and rgold i.e (Hidden state)*/
  	
    printf("\nPrinting values of Hidden state\n");
    print(&rgold[0][0], K*N);
    
    printf("\nPrinting values of Cell state\n");
    print(&cpgold[0][0], K*N);

}


return 0;
}

