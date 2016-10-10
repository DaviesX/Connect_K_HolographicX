CXX=g++

CXXFLAGS=-Og -Isrc -std=c++11
LDFLAGS=-Og

# Optimization flags
#CXXFLAGS=-Ofast -flto -Isrc -std=c++11
#LDFLAGS=-Ofast -flto -static

PROGRAM=bin/HolographicXAI

SRCS=$(wildcard src/*.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
DEPS=$(wildcard src/*.h)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(PROGRAM)

run: $(PROGRAM)
	java -jar connectk.jar cpp:$(PROGRAM)

test: $(PROGRAM)
	java -jar connectk.jar GoodAI.class cpp:$(PROGRAM)

unit: $(PROGRAM)
	gdb ./$(PROGRAM)

clean:
	rm -rf $(OBJS)
	rm -rf $(PROGRAM)
