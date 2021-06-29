#!/usr/bin/env python
# coding: utf-8

# In[11]:


import numpy as np # linear algebra
import pandas as pd # data processing, CSV file I/O (e.g. pd.read_csv)
import math
import re 


# In[12]:


# Keras 2.2.4/tensorflow 1.14.0 -> Version matching for local machine

from keras.models import Sequential
from keras.layers import Dense
from keras import optimizers
from keras import initializers
from sklearn import preprocessing

# In[13]:
# Files Prerocess 

def split_unrollFactors(row):
    t=row.split('_')
    s1=""
    for val in t[:11]:
        s1+=val+"_"
    s1=s1[:-1]
    return s1

def split_tile_sizes(row):
    t=row.split('_')
    s1=""
    for val in t[5:11]:
        s1+=val+"_"
    s1=s1[:-1]
    return s1

def processFile(fileName):
    train_df = pd.read_csv(fileName,header=None)
    
    train_df[0] = train_df.apply(lambda x : split_unrollFactors(x[0]),axis=1)
    train_df = train_df.groupby([0])
    train_df = train_df.max()

    train_df = train_df.sort_values(by=[0])
    train_df[0] = train_df.index
    
    cols = train_df.columns.tolist()
    cols = cols[-1:] + cols[:-1]
    train_df = train_df[cols] 
    
    train_df.reset_index(drop=True, inplace=True)
    return train_df

# print(processFile("matmul/matmul_explicit_data_packing.c_128_2048_4096_jt2_poly_perf.csv"))

myname = 'Gagandeep'

#Using os.walk to walk through each file in the directory provided
files = []
import os
for dirname, _, filenames in os.walk('matmul/'):
    for filename in filenames:
#         print(os.path.join(dirname, filename))
        files.append(os.path.join(dirname, filename))

# file_list = []
# unique_file_list = []    


# # For every file in the hierarchy this regex operation takes files only with N=1 or N=28

# for value in files:
#     x = re.search(".*28_poly_perf.csv$",value)
# #     x = re.search(a,value)
#     if(x):
# #         print(x.group(0))
#         file_list.append(x.group(0))
#     y = re.search("\w+/\w+/",value)
#     if(y):
#         if y.group(0) not in unique_file_list:
#             unique_file_list.append(y.group(0))

# #Sorts the ordering of files
# file_list.sort()

print(files)


# In[14]:



# In[3]:


# For particular file we are generating various permutations 
arr = []
for diff_file in files:
# for layer in range(1,20):
    train_df = processFile(diff_file)
    # train_df = pd.read_csv(diff_file,header=None)
    train_df = train_df.drop([0,1,2], axis=1)

    train_df = train_df.sort_values(by=[3],ascending=False)

    train_df = train_df.dropna(axis=1)
    # print(train_df)

    x = train_df.values #returns a numpy array
    min_max_scaler = preprocessing.MinMaxScaler()
    x_scaled = min_max_scaler.fit_transform(x)
    train_df = pd.DataFrame(x_scaled)


    # train_df[6]=0;
    # train_df[7]=0;
    # print(diff_file)
    # print(train_df)
    # exit("Stop")

    npdata = np.array(train_df)

    for i in range(len(npdata)):
        for j in range(len(npdata)):
            arr.append(np.concatenate((npdata[i],npdata[j]), axis=0))

arr = np.asarray(arr)

ver_2_GF = npdata.shape[1]

# For each data point we are creating the output as 01 or 10 and concatenating with the file
out1 = []
out2 = []

for i in range(len(arr)):
    if arr[i][0] > arr[i][ver_2_GF]:
        out1.append(1)
        out2.append(0)

    else:
        out1.append(0)
        out2.append(1)


arr = pd.DataFrame(arr)
arr = arr.drop([0,ver_2_GF],axis=1)
# arr["sum"] = arr.sum(axis=1)
# arr = arr.loc[:,[1,2,3,4,6,7,8,9]].div(arr["sum"], axis=0)

arr.head()

out1 = pd.DataFrame(out1)
out2 = pd.DataFrame(out2)

arr = pd.concat([arr, out1, out2,], axis=1, sort=False)
arr = np.array(arr)

# In[15]:



# In[4]:


# Till Here I have preprocessed the data properly.
# Splitting the data into 70% training and 30% test data.
np.random.shuffle(arr)
length = int(0.70*(len(arr)))
training_set = arr[:length]
test_set = arr[length:]


from numpy.random import seed
seed(1)
import tensorflow
tensorflow.random.set_seed(2)

model = Sequential()
# model.add(Dense(32, input_dim=8, activation='relu'))
# model.add(Dense(20, activation='relu'))
# model.add(Dense(16, activation='relu'))
# model.add(Dense(12, activation='softsign'))
# model.add(Dense(8, activation='relu'))
# model.add(Dense(2, activation='softmax'))

model.add(Dense(64, input_dim=8, activation='relu'))
model.add(Dense(32, activation='relu'))
model.add(Dense(20, activation='relu'))
model.add(Dense(16, activation='softsign'))
model.add(Dense(8, activation='relu'))
model.add(Dense(2, activation='softmax'))


# compile the keras model
model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
# fit the keras model on the dataset
model.fit(training_set[:,:-2],training_set[:,8:], epochs=100, batch_size=200)
# evaluate the keras model
_, accuracy = model.evaluate(test_set[:,:-2],test_set[:,8:])
print('Accuracy: %.2f' % (accuracy*100))


# In[24]:



# In[6]:


# Again for a file we are making all permutations and are passing through Model for prediction.
for diff_file in files:
# for layer in range(1,20):
    arr_predict = []
    train_df = processFile(diff_file)
    # train_df = pd.read_csv(diff_file,header=None)
    
    myname = "results_"+diff_file
    train_df = train_df.sort_values(by=[3],ascending=False)
    print_ver_GF = train_df.dropna(axis=1)
    print_ver_GF = np.asarray(print_ver_GF)
#     print(print_ver_GF[0,1])
    train_df = train_df.drop([0,1,2,3], axis=1)
    train_df = train_df.dropna(axis=1)

    x = train_df.values #returns a numpy array
    min_max_scaler = preprocessing.MinMaxScaler()
    x_scaled = min_max_scaler.fit_transform(x)
    train_df = pd.DataFrame(x_scaled)
    # train_df[6]=0;
    # train_df[7]=0;
    npdata = np.array(train_df)
    
    for i in range(len(npdata)):
        for j in range(len(npdata)):
            arr_predict.append(np.concatenate((npdata[i],npdata[j]), axis=0))
    arr_predict = pd.DataFrame(arr_predict)
    # arr_predict["sum"] = arr_predict.sum(axis=1)
    # arr_predict = arr_predict.loc[:,[0,1,2,3,4,5,6,7]].div(arr_predict["sum"], axis=0)
    arr_predict = np.asarray(arr_predict)
    prediction = model.predict(arr_predict)


# Uptill here we have the predicted output data ready.
# Now we calculate number of wins for each data point and append (ActualRank, GFlops, Version, wins,
# PolyRank) values in "print_result" list, for each file

    version =0
    flag=0
    count_win =0
    distinct_wins = []
    Actual_rank = 0
    print_result = []
    # for x in prediction:
    #     print(x)
    # exit("Stop")
    for values in prediction[:,0]:
        if flag<len(npdata):
            flag +=1
            if values>0.7:
                count_win+=1
        else:
            print_result.append([Actual_rank+1,print_ver_GF[Actual_rank,3],print_ver_GF[Actual_rank,0],count_win])
            Actual_rank+=1
            version+=1
            if count_win not in distinct_wins:
                distinct_wins.append(count_win)
            count_win=0
            flag=1
            if values>0.7:
                count_win+=1
    print_result.append([Actual_rank+1,print_ver_GF[Actual_rank,3],print_ver_GF[Actual_rank,0],count_win])
    if count_win not in distinct_wins:
        distinct_wins.append(count_win)
    
# On the basis of Number of wins, We are calculating the number of Polyrank for each data point.
    distinct_wins.sort(reverse = True)
    polyrank = []
    for values in print_result:
        polyrank.append(distinct_wins.index(values[3])+1)
    polyrank = pd.DataFrame(polyrank)
    Final = pd.concat([pd.DataFrame(print_result),polyrank], axis=1, sort=False)
    Final= np.asarray(Final)

    temp = Final[Final[:,4] == 1]

    num_row_in_top5_per = math.ceil((Final.shape[0])*0.05)
    rank = 1

# Calculating best in Top 5%
    top5_max_list =[]    
    while num_row_in_top5_per>0:
        temp_rank = Final[Final[:,4] == rank]
        top5_max_list.append(np.max(temp_rank[:,1]))
        num_row_in_top5_per-=temp_rank.shape[0]
        rank+=1
        
    top5_perf_list = [np.max(Final[:,1]),np.max(temp[:,1]),Final.shape[0],max(top5_max_list),np.min(Final[:,1]),np.median(Final[:,1])]
    # print("Hello",top5_perf_list)

# Writing the Wins, polyrank etc to a file.
    f = open(myname, "a+") 
    f.write('ActualRank, GFlops, Version, wins, PolyRank,\n')
    f.close()
    for values in Final:
        f = open(myname, "a+")        
        str_temp = ""
        for val in values:
            str_temp = str_temp + str(val)+", "
        f.write("%s\n" % str_temp)
        f.close()

# Creating summary files  and writing it to a file
    myname_summary = "summary_results_matmul"
#     string2 = re.search("\w+/\w+/",diff_file).group(0)
#     if string2 in diff_file:
#         string2 = diff_file.replace(string2,'')
#     if "_poly_perf.csv" in string2:
#         string2 = string2.replace("_poly_perf.csv",'')
        
    f = open(myname_summary, "a")
#     f.write("Max_GFLOPS, Poly_Top_1GFLOPS,numVariants,Poly_Top_0.050000,Min_GFLOPS,Median_GFLOPS,\n")
    string2=diff_file
    str_temp = string2+", "
    for val in top5_perf_list:
        str_temp = str_temp + str(val) + ", "
    f.write("%s\n" % str_temp)
    f.close()

