# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
"""
.. _tune_relay_x86:

Auto-tuning a convolutional network for x86 CPU
===============================================
**Author**: `Yao Wang <https://github.com/kevinthesun>`_, `Eddie Yan <https://github.com/eqy>`_

This is a tutorial about how to tune convolution neural network
for x86 CPU.
"""
import os
import numpy as np

import tvm
from tvm import autotvm
from tvm import relay
from tvm.relay import testing
from tvm.autotvm.tuner import XGBTuner, GATuner, RandomTuner, GridSearchTuner
from tvm.autotvm.graph_tuner import DPTuner, PBQPTuner
import tvm.contrib.graph_runtime as runtime
from tvm.relay.testing import layers

from tvm.relay.testing.init import create_workload
#################################################################
# Define network
# --------------
# First we need to define the network in relay frontend API.
# We can either load some pre-defined network from :code:`relay.testing`
# or building :any:`relay.testing.resnet` with relay.
# We can also load models from MXNet, ONNX and TensorFlow.
#
# In this tutorial, we choose resnet-18 as tuning example.


def get_network(name, batch_size, iters, ifw, ifh, orig_nIfm, orig_nOfm, kw, kh, pad_w, pad_h, stride):
    """Get the symbol definition and random weight of a network"""
    ofh = (ifh + 2 * pad_h - kh) / stride + 1
    ofw = (ifw + 2 * pad_w - kw) / stride + 1
    input_shape = (batch_size, orig_nIfm, ifh, ifw)
    output_shape = (batch_size, orig_nOfm, ofh, ofw)

    if "resnet" in name:
        n_layer = int(name.split('-')[1])
        mod, params = relay.testing.resnet.get_workload(num_layers=n_layer, batch_size=batch_size, dtype=dtype)
    elif name == 'custom':
        layout="NCHW" 
        dtype="float32"
        data_shape = [batch_size, orig_nIfm, ifh, ifw]
        data = relay.var("data", shape=data_shape, dtype=dtype) 
        net = layers.conv2d( data=data, channels=orig_nOfm, kernel_size=(kh, kw),strides=(stride, stride), padding=(pad_h, pad_w), name="conv0",data_layout=layout, kernel_layout="OIHW")
        mod, params = create_workload(relay.Function(relay.analysis.free_vars(net), net))
    else:
        raise ValueError("Unsupported network: " + name)

    return mod, params, input_shape, output_shape


# Replace "llvm" with the correct target of your CPU.
# For example, for AWS EC2 c5 instance with Intel Xeon
# Platinum 8000 series, the target should be "llvm -mcpu=skylake-avx512".
# For AWS EC2 c4 instance with Intel Xeon E5-2666 v3, it should be
# "llvm -mcpu=core-avx2".
target = "llvm -mcpu=cascadelake"

batch_size =28 
dtype = "float32"
model_name = "custom"
log_file = "%s.log" % model_name
graph_opt_sch_file = "%s_graph_opt.log" % model_name

# Set the input name of the graph
# For ONNX models, it is typically "0".
input_name = "data"

# Set number of threads used for tuning based on the number of
# physical CPU cores on your machine.
num_threads = batch_size
os.environ["TVM_NUM_THREADS"] = str(num_threads)


#################################################################
# Configure tensor tuning settings and create tasks
# -------------------------------------------------
# To get better kernel execution performance on x86 CPU,
# we need to change data layout of convolution kernel from
# "NCHW" to "NCHWc". To deal with this situation, we define
# conv2d_NCHWc operator in topi. We will tune this operator
# instead of plain conv2d.
#
# We will use local mode for tuning configuration. RPC tracker
# mode can be setup similarly to the approach in
# :ref:`tune_relay_arm` tutorial.

tuning_option = {
    'log_filename': log_file,
    'tuner': 'random',
    'early_stopping': None,

    'measure_option': autotvm.measure_option(
        builder=autotvm.LocalBuilder(),
        runner=autotvm.LocalRunner(number=10, repeat=1,
                                   min_repeat_ms=1000),
    ),
}


# You can skip the implementation of this function for this tutorial.
def tune_kernels(tasks,
                 measure_option,
                 tuner='gridsearch',
                 early_stopping=None,
                 log_filename='tuning.log'):

    for i, task in enumerate(tasks):
        prefix = "[Task %2d/%2d] " % (i+1, len(tasks))

        # create tuner
        if tuner == 'xgb' or tuner == 'xgb-rank':
            tuner_obj = XGBTuner(task, loss_type='rank')
        elif tuner == 'ga':
            tuner_obj = GATuner(task, pop_size=50)
        elif tuner == 'random':
            tuner_obj = RandomTuner(task)
        elif tuner == 'gridsearch':
            tuner_obj = GridSearchTuner(task)
        else:
            raise ValueError("Invalid tuner: " + tuner)

        # do tuning
        n_trial=len(task.config_space)
        tuner_obj.tune(n_trial=n_trial,
                       early_stopping=early_stopping,
                       measure_option=measure_option,
                       callbacks=[
                           autotvm.callback.progress_bar(n_trial, prefix=prefix),
                           autotvm.callback.log_to_file(log_filename)])


# Use graph tuner to achieve graph level optimal schedules
# Set use_DP=False if it takes too long to finish.
def tune_graph(graph, dshape, records, opt_sch_file, use_DP=True):
    target_op = [relay.op.get("nn.conv2d"),]
    Tuner = DPTuner if use_DP else PBQPTuner
    executor = Tuner(graph, {input_name: dshape}, records, target_op, target)
    executor.benchmark_layout_transform(min_exec_num=2000)
    executor.run()
    executor.write_opt_sch2record_file(opt_sch_file)


########################################################################
# Finally, we launch tuning jobs and evaluate the end-to-end performance.

def tune_and_evaluate(tuning_opt, iters, ifw, ifh, orig_nIfm, orig_nOfm, kw, kh, pad_w, pad_h, stride):
    # extract workloads from relay program
    print("Extract tasks...")
    mod, params, data_shape, out_shape = get_network(model_name, batch_size, iters, ifw, ifh, orig_nIfm, orig_nOfm, kw, kh, pad_w, pad_h, stride)
    tasks = autotvm.task.extract_from_program(mod["main"], target=target,
                                              params=params,
                                              ops=(relay.op.get("nn.conv2d"),))

    print(tasks)
    # run tuning tasks
    tune_kernels(tasks, **tuning_opt)
    tune_graph(mod["main"], data_shape, log_file, graph_opt_sch_file)


# We do not run the tuning in our webpage server since it takes too long.
# Uncomment the following line to run it by yourself.

import sys
argCount = len(sys.argv)
index = 1
if argCount > index :
    iters = int(sys.argv[index])
    index +=1
if argCount > index :
    ifw = int(sys.argv[index])
    index +=1
if argCount > index :
    ifh = int(sys.argv[index])
    index +=1
if argCount > index :
    orig_nIfm = int(sys.argv[index])
    index +=1
if argCount > index :
    orig_nOfm = int(sys.argv[index])
    index +=1
if argCount > index :
    kw = int(sys.argv[index])
    index +=1
if argCount > index :
    kh = int(sys.argv[index])
    index +=1
if argCount > index :
    pad_w = int(sys.argv[index])
    index +=1
if argCount > index :
    pad_h = int(sys.argv[index])
    index +=1
if argCount > index :
    stride = int(sys.argv[index])
    index +=1
if argCount > index :
    config_num = sys.argv[index]
    index +=1


tune_and_evaluate(tuning_option, iters, ifw, ifh, orig_nIfm, orig_nOfm, kw, kh, pad_w, pad_h, stride)

