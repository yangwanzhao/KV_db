g++ client_kv.cc \
	-std=c++17 \
	-lboost_system \
	-lboost_regex \
	-lboost_serialization \
	-lv8 \
	-lpthread \
	-lstdc++ \
	-o client
	
gcc server_kv.cc hashlist.cc\
    -std=c++11 \
    -lboost_system \
    -lboost_regex \
    -lboost_serialization \
    -lv8 \
    -lpthread \
    -lstdc++ \
    -o server