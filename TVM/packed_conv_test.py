import d2ltvm
import numpy as np
import tvm
from tvm import te
import os

os.environ['KMP_AFFINITY']='granularity=fine,noduplicates,compact,1,0'
target = 'llvm'
num_threads = 1
os.environ["TVM_NUM_THREADS"] = str(num_threads)

def conv_pack(oc, ic, nh, nw, kh, kw, ph, pw, toc, tic):
    """Pack data and weight for convolution

    oc, ic : output and input channels
    nh, nw : input width and height
    kh, kw : kernel width and height
    ph, pw : height and width padding
    toc, tic : the tiling sizes of the output and input channels
    """
    X = te.placeholder((ic//tic, nh+ph*2, nw+pw*2,tic), name='X')
    K = te.placeholder((oc//toc, ic//tic, kh, kw,tic,toc), name='K')
    # pack X and K
    return X,K

def conv(oc, ic, nh, nw, kh, kw, ph, pw, sh, sw, toc, tic):
    """2-D conv

    oc, ic : output and input channels.
    nh, nw : input width and height
    kh, kw : kernel width and height
    ph, pw : height and width padding
    sh, sw : height and width strides
    toc, tic : the tiling sizes of output channel and input channel
    """
    PackedX, PackedK = conv_pack(
        oc, ic, nh, nw, kh, kw, ph, pw, toc, tic)
    # reduction axes
    ric_in = te.reduce_axis((0, tic), name='ric_in')
    ric_out = te.reduce_axis((0, ic//tic), name='ric_out')
    rkh = te.reduce_axis((0, kh), name='rkh')
    rkw = te.reduce_axis((0, kw), name='rkw')
    # output height and weights
    oh = d2ltvm.conv_out_size(nh, kh, ph, sh)
    ow = d2ltvm.conv_out_size(nw, kw, pw, sw)
    # Compuated Y in the packed layout
    PackedY = te.compute(
        (oc//toc, oh, ow, toc),
        lambda oc_out, x, y, oc_in: te.sum(
            PackedX[ric_out, x*sh+rkh, y*sw+rkw, ric_in] *
            PackedK[oc_out, ric_out, rkh, rkw, ric_in, oc_in],
            axis=[ric_out, rkh, rkw, ric_in]), name='PackedY')
    return PackedX, PackedK, PackedY


# tiling sizes for output channel, input channel, and width
toc, tic, tw = 16, 16, 4

def cached_block(oc, ic, n, k, p, s):
    PackedX, PackedK, PackedY = conv(
        oc, ic, n, n, k, k, p, p, s, s, toc, tic)
    print("cache_block_sizes ",oc,ic,n,k,p,s,toc,tic)
    s = te.create_schedule(PackedY.op)
    CachedY = s.cache_write(PackedY, 'local')
    oc_out, h, w, oc_in = s[PackedY].op.axis
    oc_out_h = s[PackedY].fuse(oc_out, h)
    # Parallel on the first two dimensions oc_out and h
    s[PackedY].parallel(oc_out_h)
    # Optimize the computation of a cached output block
    w_out, w_in = s[PackedY].split(w, factor=tw)  # Split the columns
    s[CachedY].compute_at(s[PackedY], w_out)
    _, _, cw, oc_in = CachedY.op.axis
    ric_out, rkh, rkw, ric_in = CachedY.op.reduce_axis
    s[CachedY].reorder(ric_out, rkh, rkw, ric_in, cw, oc_in)
    s[CachedY].unroll(ric_in)
    s[CachedY].unroll(cw)
    s[CachedY].vectorize(oc_in)
    return s, (PackedX, PackedK, PackedY)

#s, args = cached_block(32, 32, 64, 3, 1, 1)
# Uncomment the following line to see the long
# psuedo codes because of unrolling.
# tvm.lower(s, args, simple_mode=True)

def bench_conv_tvm(func, sizes, target):
    def workload(nrepeats):
        print("Trying to print mod.entry_name",mod.entry_name)
        timer = mod.time_evaluator(mod.entry_name, ctx=ctx, number=nrepeats)
        return timer(x, k, y).mean * nrepeats
    gflops, times = [], []
    for (c, n, k) in sizes:
        args = c, c, n, k, (k-1)//2, 1 # oc, ic, n, k, p, s
        s, (X, K, Y) = func(*args)
        mod = tvm.build(s, [X, K, Y], target)
        ctx = tvm.context(target, 0)
        x, k, y = get_conv_data(
            *args, lambda x: tvm.nd.array(x, ctx=ctx))
        print("Calling time append")
        times.append(d2ltvm.bench_workload(workload))
        print("Calling the gflops  function")
        gflops.append(d2ltvm.conv_gflop(*args))
    return np.array(gflops) / np.array(times)

# Save to the d2ltvm package.
def get_conv_data(oc, ic, n, k, p=1, s=1, constructor=None, conv_type='direct'):
    """Return random 3-D data tensor, 3-D kernel tenor and empty 3-D output
    tensor with the shapes specified by input arguments.

    oc, ic : output and input channels
    n : input width and height
    k : kernel width and height
    p : padding size, default 0
    s : stride, default 1
    conv_type: either direct 2D or depthwise, default direct
    constructor : user-defined tensor constructor
    """
    np.random.seed(0)
    data = np.random.normal(size=(ic//tic, n+2*p, n+2*p,tic)).astype('float32')
    ic_weight = ic
    weight = np.random.normal(size=(oc//toc, ic_weight//tic, k, k,tic,toc)).astype('float32')
    on = d2ltvm.conv_out_size(n, k, p, s)
    out = np.empty((oc//toc, on, on,toc), dtype='float32')
    if constructor:
        data, weight, out = (constructor(x) for x in [data, weight, out])
    print("Data ",data.shape)
    print("Weight ",weight.shape)
    print("out ",out.shape)
    return data, weight, out


channels = 2**np.arange(4, 9)
sizes = [(int(c), 64, 3) for c in channels] # a list of (c, n, k)
tvm_gflops = bench_conv_tvm(cached_block, sizes, target)
print(tvm_gflops)
print(sizes)
