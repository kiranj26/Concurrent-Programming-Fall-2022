#!/bin/sh
# ################################# #
#  Auto testing shell for Execution #
# ################################# #

# Remove all object files and compile again
make clean

# Rebuild Again 

make

# clean screen
clear

##########################################
# Execution of Concurrent Data Structure #
##########################################
Out_string="---Running With Concurrent Data Structure---\n"
echo "$Out_string"

# set no of threads and no of iterations to inal out file
Out_string="Running With Scenrio 1\n"
echo "$Out_string"

THREADCOUNT="5"
ITERATIONS="100000"
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLStack 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLQueue 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=MSQ 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=Treiber 

Out_string="\nRunning With Scenrio 2\n"
echo "$Out_string"

THREADCOUNT="10"
ITERATIONS="100000"
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLStack 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLQueue 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=MSQ 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=Treiber 

Out_string="\nRunning With Scenrio 3\n"
echo "$Out_string"

THREADCOUNT="20"
ITERATIONS="100000"
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLStack 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=SGLQueue 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=MSQ 
./finalproject -t $THREADCOUNT -i $ITERATIONS --DS=Treiber 

########################
# Execution of counter #
########################

Out_string="\n--------Running For Counter--------------- \n"
echo "$Out_string"
Out_string="--------Running For Scenario 1--------------- \n"
echo "$Out_string"

Out_string="\nRunning For Counter With TAS \n"
echo "$Out_string"
# With TAS

./counter -t 5 -i 100000 --lock=tas -o dest.txt

Out_string="\nRunning For Counter With FIFO \n"
echo "$Out_string"
# With FIFO
./counter -t 5 -i 100000 --lock=mcs -o dest.txt


Out_string="\nRunning For Counter With LIFO \n"
echo "$Out_string"
# With LIFO
./counter -t 5 -i 100000 --lock=LIFO -o dest.txt

Out_string="\n--------Running For Scenario 2--------------- \n"
echo "$Out_string"

Out_string="\nRunning For Counter With TAS \n"
echo "$Out_string"
# With TAS

./counter -t 10 -i 100000 --lock=tas -o dest.txt

Out_string="\nRunning For Counter With FIFO \n"
echo "$Out_string"
# With FIFO
./counter -t 10 -i 100000 --lock=mcs -o dest.txt


Out_string="\nRunning For Counter With LIFO \n"
echo "$Out_string"
# With LIFO
./counter -t 10 -i 100000 --lock=LIFO -o dest.txt

