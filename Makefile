
KINESIS_FLAG = --std=c++11 -laws-cpp-sdk-kinesis -laws-cpp-sdk-core
KS_SRC = src/base.cpp src/client.cpp src/ks_kinesis.cpp src/ks_script.cpp src/tools.cpp
LUA_FLAG = -llua -ldl -lm

.PHONY: all clean

all: kinesis test

client:
	g++ -o bin/client ${KS_SRC} ${KINESIS_FLAG} ${LUA_FLAG}

kinesis:
	g++ -o bin/ks_kinesis src/ks_kinesis.cpp --std=c++11 -laws-cpp-sdk-kinesis -laws-cpp-sdk-core

test:
	g++ -o bin/ks_test src/base.cpp src/ks_kinesis.cpp src/test_ks_kinesis.cpp ${KINESIS_FLAG}
	g++ -o bin/timer_test src/ks_timer.cpp src/test_ks_timer.cpp --std=c++11

clean:
	@rm bin/ks_kinesis
	@rm bin/desc
	@rm bin/load_test
	@rm bin/ks_test
