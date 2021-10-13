
CXX       := g++  
CC		  := gcc
LD_BINARY := ld
BUILD    := ./build
CXXFLAGS :=  -std=gnu++14 -O2 -Wall -Wextra -Wno-comment  -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-parameter -fpermissive 
LDFLAGS  :=  -L/usr/lib/x86_64-linux-gnu/openmpi/lib -lmpi -lmpi_cxx
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
RUN_DIR  := run
INCLUDE  :=  -Iinclude -I/usr/local/include -I/usr/lib/x86_64-linux-gnu/openmpi/include -I./include
SRC      := $(wildcard src/*.cpp)

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/SyntheticDynamicMPI 


$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/SyntheticDynamicMPI: $(OBJECTS) apps/main.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS)  $(INCLUDE)   -o $(APP_DIR)/SyntheticDynamicMPI $^ $(LDFLAGS)

-include $(DEPENDENCIES)

#.PHONY: all build clean debug release info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(RUN_DIR)


clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*


#pi: $(APP_DIR)/GARDIS FORCE
#	scp $(APP_DIR)/* pi:/tmp/ && ssh pi sudo pkill -f GARDIS && ssh pi sudo /tmp/GARDIS




run: $(APP_DIR)/SyntheticDynamicMPI FORCE
	cd run; mpirun --oversubscribe -np 1 ../$(APP_DIR)/SyntheticDynamicMPI 


format: FORCE
	find src -type f | xargs clang-format-10 -style='{ColumnLimit: 150}' -i
	find include -type f | xargs clang-format-10 -style='{ColumnLimit: 150}' -i
	find apps -type f | xargs clang-format-10 -style='{ColumnLimit: 150}' -i
	
FORCE: ;