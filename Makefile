CXX=g++
CFLAGS=-g -I.
CXXFLAGS=-g -I. -std=c++14
LDFLAGS=
PROGRAM=aishell

SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
DEPS=$(wildcard *.h)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(PROGRAM)

run: $(PROGRAM)
	java -jar connectk.jar cpp:aishell

clean:
	rm -rf $(OBJS)
	rm -rf $(PROGRAM)
