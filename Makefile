CXX=g++
CFLAGS=-g -I.
LDFLAGS=
PROGRAM=aishell

SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
DEPS=$(wildcard *.h)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(PROGRAM)

$(SRCS): $(DEPS)
	$(CXX) -c $(CFLAGS) $<

clean:
	rm -rf $(OBJS)
	rm -rf $(PROGRAM)
