CXX=g++
CXXFLAGS=-Og -Isrc -std=c++11
LDFLAGS=-Og
PROGRAM=bin/HolographicXAI

SRCS=$(wildcard src/*.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRCS))

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(PROGRAM)

run: $(PROGRAM)
	java -jar connectk.jar cpp:$(PROGRAM)

clean:
	rm -rf $(OBJS)
	rm -rf $(PROGRAM)
