g++ kvserver.cc hashlist.cpp \
	-std=c++17 \
	-lboost_system \
	-lboost_regex \
	-lboost_serialization \
	-lv8 \
	-lpthread \
	-o KV
	