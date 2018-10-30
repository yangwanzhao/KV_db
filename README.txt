# READ ME
################ how to compile the code #################
# after using dockerfile to create the container
# please go to root/ and run below code to download boost 1.66
rm -f /usr/lib/libboost*
rm -fr 'find / -name libboost*'
# remove exiting boost head file
mv /usr/include/boost /usr/include/boost-bak
# download wget
apt-get install wget
# download Boost
wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
tar -zxvf boost_1_66_0.tar.gz
cd boost_1_66_0
./bootstrap.sh. 
./b2 install  # nearly 30 mins (which is bad)

# no need for Makefile
# to compile the code, just use run.sh
. run.sh
# server and client will be generated
# use screen and open two windows to run server and client respectively
# ./server
# ./client

################ how to use the key-value store #################
# for test when I am coding, I used some simple code to initialize (INIT) hash table
# and put some kv-pairs into it, and give a function (SHOW) to show the table in server
# Also, (TESTREDUCE) can be given by client to server to easily test the REDUCE code

# Logic for REDUCE part
# filter and combine function are provided by client, so client should give the name of 
# these two function (like ‘fil.js’ and ‘com.js’ in the dir)
# client program will load the files and send the content(script) to server
# server run filter function on each key, and put the required value to a vector based on
# the return value of filter
# vector is put into a char[] by using a specific protocol and char[] will be given to 
# combine function, thus a result can be given back to c++ and is sent back to client

# it is noticeable that a filr "Readonly_Server.js" is loaded and added to the top of 
# combine script, in order to parse the protocol of value vector. In this way, client 
# do not need to parse the vector but only get a interface of myValue array

################ Existing Files #################
# "Readonly_Server.js" 	: parse the values
# "client" 				: compiled client exe
# "client_kv.cc"		: client c++ code
# "com.js"				: example for combine function javascript
# "common.h"			: head file for v8 code
# "fil.js"				: example for filter function javascript
# "hashlist.cc"			: code for hashlist
# "hashlist.h"			: head file for hashlist
# "print.h"				: head file for JS to invoke c++ print function
# "run.sh"				: use ". run.sh" to compile the code
# "server"				: compiled server exe
# "server_kv.cc"		: server c++ code