# Compiler configurations
CXX=g++

CXXFLAGS=-g -Isrc -std=c++11
LDFLAGS=-g

# Optimization flags
#CXXFLAGS+=-Ofast -flto
#LDFLAGS+=-Ofast -flto

# Output executable and default target.
BIN=bin/HolographicXAI
.DEFAULT_GOAL=$(BIN)

# Source directory
SRC_DIR=src/

# Source files, object files and dependencies.
SRCS=$(wildcard $(SRC_DIR)*.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRCS))

# Include all dependency targets
DEPS=$(patsubst %.cpp, %.d, $(SRCS))
DF=$(SRC_DIR)$(*F)
-include $(DEPS)


# Color scheme
CL="\033[92m"
CR="\033[0m"

$(BIN): $(OBJS)
	@echo $(CL)"Linking into" $(BIN) $(CR)
	@$(CXX) $(OBJS) $(LDFLAGS) -o $(BIN)
	@echo $(CL)"Done."$(CR)

%.o: %.cpp
	@# Build the dependency file
	@$(CXX) -MM -MP -MT $(DF).o -MT $(DF).d $(CXXFLAGS) $< > $(DF).d 
	@# Compile the object file
	@echo $(CL)"Compiling" $<" => "$@$(CR)
	@$(CXX) -c $< $(CXXFLAGS) -o $@

run: $(BIN)
	java -jar connectk.jar cpp:$(BIN)

test: $(BIN)
	java -jar connectk.jar GoodAI.class cpp:$(BIN)

unit: $(BIN)
	gdb ./$(BIN)

clean:
	@echo $(CL)"Removing object files..."$(CR)
	@rm -f $(OBJS)
	@echo $(CL)"Removing dependency files.."$(CR)
	@rm -f $(DEPS)
	@echo $(CL)"Removing executable..."$(CR)
	@rm -f $(BIN)
	@echo $(CL)"Done."$(CR)

