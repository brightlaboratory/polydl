import sys
import os
import tvm
from tvm import te
import numpy
import timeit
import pandas as pd

# The default tensor type in tvm
dtype = "float32"

# using Intel AVX2(Advanced Vector Extensions) ISA for SIMD
# To get the best performance, please change the following line
# to llvm -mcpu=cascadelake, or specific type of CPU you use
target = 'llvm  -mcpu=cascadelake'
ctx = tvm.context(target, 0)

num_threads = 32
os.environ["TVM_NUM_THREADS"] = str(num_threads)

# Taking Command line arguments
bf1 = int(sys.argv[1])
bf2 = int(sys.argv[2])
factor = int(sys.argv[3])
M = int(sys.argv[4])
N = int(sys.argv[5])
K = int(sys.argv[6])

a = tvm.nd.array(numpy.random.rand(M, K).astype(dtype), ctx)
b = tvm.nd.array(numpy.random.rand(K, N).astype(dtype), ctx)

np_repeat = 100
np_runing_time = timeit.timeit(setup='import numpy\n'
                                        'M = ' + str(M) + '\n'
                                        'K = ' + str(K) + '\n'
                                        'N = ' + str(N) + '\n'
                                        'dtype = "float32"\n'
                                        'a = numpy.random.rand(M, K).astype(dtype)\n'
                                        'b = numpy.random.rand(K, N).astype(dtype)\n',
                                stmt='answer = numpy.dot(a, b)',
                                number=np_repeat)
# print("Numpy running time: %f" % (np_runing_time / np_repeat))

answer = numpy.dot(a.asnumpy(), b.asnumpy())


# AutoTVM Algorithm of GEMM
k = te.reduce_axis((0, K), 'k')
A = te.placeholder((M, K), name='A')
B = te.placeholder((K, N), name='B')

packedB = te.compute((N / bf2, K, bf2), lambda x, y, z: B[y, x * bf2 + z], name='packedB')
C = te.compute((M, N),
            lambda x, y: te.sum(A[x, k] * packedB[y // bf2, k, tvm.tir.indexmod(y, bf2)], axis=k),
            name = 'C')

s = te.create_schedule(C.op)

CC = s.cache_write(C, 'global')

xo, yo, xi, yi = s[C].tile(C.op.axis[0], C.op.axis[1], bf1, bf2)

s[CC].compute_at(s[C], yo)

xc, yc = s[CC].op.axis

k, = s[CC].op.reduce_axis
ko, ki = s[CC].split(k, factor)
s[CC].reorder(ko, xc, ki, yc)
s[CC].unroll(ki)
s[CC].vectorize(yc)

# parallel
s[C].parallel(xo)

x, y, z = s[packedB].op.axis
s[packedB].vectorize(z)
s[packedB].parallel(x)

func = tvm.build(s, [A, B, C], target=target, name = 'mmult')
assert func

c = tvm.nd.array(numpy.zeros((M, N), dtype = dtype), ctx)
func(a, b, c)
tvm.testing.assert_allclose(c.asnumpy(), answer, rtol=1)

evaluator = func.time_evaluator(func.entry_name, ctx, number=1)
opt6_time = evaluator(a, b, c).mean
print('TVM-Opt: %f' % opt6_time)
NumOps = 2 * M*N*K # Why K/2?
gflops = 1.0e-9 * NumOps / opt6_time
# npglfops = 1.0e-9 * NumOps / (np_runing_time / np_repeat)
# print("numpy GFLOPS ", npglfops)
print("GFLOPS ",gflops)
