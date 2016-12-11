

.PHONY: all clean

all: desc kinesis test

desc:
	g++ -o bin/desc src/describle.cpp --std=c++11 -laws-cpp-sdk-kinesis -laws-cpp-sdk-core

kinesis:
	g++ -o bin/ks_kinesis src/ks_kinesis.cpp --std=c++11 -laws-cpp-sdk-kinesis -laws-cpp-sdk-core

test:
	g++ -o bin/load_test src/load_run.cpp src/test_load_run.cpp --std=c++11 -llua -lm -ldl

clean:
	@rm bin/ks_kinesis
	@rm bin/desc
	@rm bin/load
