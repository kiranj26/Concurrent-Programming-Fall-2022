#!/bin/sh

# ##################################### #
#  Auto testing shell for Perf Analysis #
# ##################################### #

# Remove all object files and compile again
make clean

# clean screen
clear

# Rebuild Again 

make

# set no of threads and no of iterations to inal out file
THREADCOUNT="5"
ITERATIONS="10000"
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLStack 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLQueue 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=MSQ 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=Treiber 

THREADCOUNT="10"
ITERATIONS="10000"
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLStack 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLQueue 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=MSQ 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=Treiber 

THREADCOUNT="20"
ITERATIONS="10000"
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLStack 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLQueue 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=MSQ 
sudo perf stat ./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=Treiber 