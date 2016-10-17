CXX=g++

CXXFLAGS=-Og -Isrc -std=c++11
LDFLAGS=-Og

# Optimization flags
#CXXFLAGS=-Ofast -flto -Isrc -std=c++11
#LDFLAGS=-Ofast -flto -static

PROGRAM=bin/HolographicXAI

SRC_DIR=src/
SRCS=$(wildcard $(SRC_DIR)*.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
DEPS=$(patsubst %.cpp, %.d, $(SRCS))
DF=$(SRC_DIR)$(*F)
CL="\033[92m"
CR="\033[0m"

-include $(DEPS)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	@echo $(CL)"Linking into" $(PROGRAM) $(CR)
	@$(CXX) $(OBJS) $(LDFLAGS) -o $(PROGRAM)
	@echo $(CL)"Done."$(CR)

%.o: %.cpp
	@# Build the dependency file
	@$(CXX) -MM -MP -MT $(DF).o -MT $(DF).d $(CXXFLAGS) $< > $(DF).d 
	@# Compile the object file
	@echo $(CL)"Compiling" $<" => "$@$(CR)
	@$(CXX) -c $< $(CXXFLAGS) -o $@

run: $(PROGRAM)
	java -jar connectk.jar cpp:$(PROGRAM)

test: $(PROGRAM)
	java -jar connectk.jar GoodAI.class cpp:$(PROGRAM)

unit: $(PROGRAM)
	gdb ./$(PROGRAM)

clean:
	@echo $(CL)"Removing object files..."$(CR)
	@rm -f $(OBJS)
	@echo $(CL)"Removing dependency files.."$(CR)
	@rm -f $(DEPS)
	@echo $(CL)"Removing executable..."$(CR)
	@rm -f $(PROGRAM)
	@echo $(CL)"Done."$(CR)

