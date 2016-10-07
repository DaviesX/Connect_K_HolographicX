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

$(SRCS): 
	$(CXX) -c $(CFLAGS) $<

run: $(PROGRAM)
	java -jar connectk.jar cpp:aishell

clean:
	rm -rf $(OBJS)
	rm -rf $(PROGRAM)
