TARGET := tests_runner
BUILD_DIR := build
PROJECT_DIR := .

# Find all the C++ files we want to compile.
SRCS := $(shell find $(PROJECT_DIR) -name *.cpp | sort)

# Find all already compiled object files, they are needed to improve compilation time.
PRECOMPILED_OBJS := $(shell find $(BUILD_DIR) -name *.o)

# Make a list of target object files by string substitution for every C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:$(PROJECT_DIR)/%=$(BUILD_DIR)/%.o) $(PRECOMPILED_OBJS)

# Make a list of dependency files by string substitution for each object file.
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./ need to be passed to GCC so that it can find header files.
INC_DIRS := $(shell find $(PROJECT_DIR) -type d | sort)

# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I/usr/local/include/catch2

# Set compiler, compiler flags and libs that will be used for compilation.
CXX := g++
CXXFLAGS := -O2 -std=c++20 -pthread
CXXLIBS := -lcpprest -lboost_system -lssl -ljsoncpp -lcrypto

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP

all: $(BUILD_DIR)/$(TARGET)

# The final build step.
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(CXXLIBS)

# Build step for C++ sources.
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ $(CXXLIBS)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
