TARGET := mp4parser
LIB_NAME := libmp4parser.so
SRC_DIRS := src
CXX := g++
ECHO := echo
RM := rm -rf
MKDIR := mkdir -p

OBJ_DIR := obj
BIN_DIR := bin
LIB_DIR := lib
COMILE_GOAL :=

SRC_FILES := $(foreach dir, $(SRC_DIRS), $(notdir $(wildcard $(dir)/*.cpp)))
OBJ_FILES := $(patsubst %.cpp, %.o, $(SRC_FILES))

ifeq ($(MAKECMDGOALS), release)
    CXXFLAGS := -D_RELEASE_VERSION_ -D_DEBUG_LEVEL_=3 -O2
	COMILE_GOAL := release
else
    CXXFLAGS := -D_DEBUG_ALL_PRINT_BUFFER_ON_ -D_DEBUG_LEVEL_=0 -Wall -Wformat -g
	COMILE_GOAL := debug
endif

OBJ_DIR := $(OBJ_DIR)/$(COMILE_GOAL)
OBJ_FULL_PATH_FILES := $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

BIN_DIR := $(BIN_DIR)/$(COMILE_GOAL)
LIB_DIR := $(LIB_DIR)/$(COMILE_GOAL)

FULL_PATH_LIB_FILE  := $(addprefix $(LIB_DIR)/, $(LIB_NAME))
	
vpath %.cpp $(SRC_DIRS)
vpath %.o   $(OBJ_DIR)

CXXFLAGS += -I./include -I./ -std=c++0x -Wl,-rpath=$(LIB_DIR)

.PHONY:all COMPILE_LIB COMPILE_EXECUTE MK_OBJ_DIR

all release:MK_ALL_DIRS COMPILE_LIB COMPILE_EXECUTE

clean:
	@$(ECHO) "Clean object files..."
	@$(RM) $(OBJ_DIR)/*
	@$(ECHO) "Clean so file..."
	@$(RM) $(LIB_DIR)/*
	@$(ECHO) "Clean execute file..."
	@$(RM) $(BIN_DIR)/*
	@$(ECHO) "Clean done!"


MK_ALL_DIRS:
	@$(MKDIR) $(OBJ_DIR)
	@$(MKDIR) $(LIB_DIR)
	@$(MKDIR) $(BIN_DIR)
	
COMPILE_LIB:$(OBJ_FILES)
	@$(ECHO) "Compiling libmp4parser.so ..."
	$(CXX)  -shared $(OBJ_FULL_PATH_FILES) -o $(FULL_PATH_LIB_FILE)
	@$(ECHO) "Done!"
	@$(ECHO) 
	
DEBUG_SHOW_VALUE:
	@$(ECHO) "========================SRC File============================="
	@$(ECHO) $(SRC_FILES)
	@$(ECHO) "========================================================"
	@$(ECHO)
	@$(ECHO) "========================OBJ File============================="
	@$(ECHO) $(OBJ_FILES)
	@$(ECHO) "========================================================"
	@$(ECHO)
	
    
COMPILE_EXECUTE:main.o
	@$(ECHO) "Compiling mp4parser ..." 
	@$(CXX) $(CXXFLAGS)  -o $(OBJ_DIR)/main.o -c main.cpp
	@$(CXX) $(CXXFLAGS) $(OBJ_DIR)/main.o -o $(BIN_DIR)/$(TARGET) -L$(LIB_DIR) -lmp4parser
	@$(ECHO) "Done!"
	@$(ECHO)
	
%.o:%.cpp
	@$(ECHO) Compiling:$<...  
	@$(CXX) $(CXXFLAGS) -fPIC -o $(OBJ_DIR)/$@ -c $<
	@$(ECHO) done!
	@$(ECHO)


