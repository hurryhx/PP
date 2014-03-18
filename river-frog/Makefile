#
# $File: Makefile
# $Date: Tue Sep 03 00:31:22 2013 +0000
#
# A single output portable Makefile for
# simple c++ project

OBJ_DIR = obj
BIN_DIR = bin
TARGET = river-and-frog

BIN_TARGET = $(BIN_DIR)/$(TARGET)


PKGCONFIG_LIBS = gtkmm-3.0 cairomm-1.0
INCLUDE_DIR =

DEFINES = -DUSE_PTHREAD #-D__DEBUG_BUILD  #-D__SIMPLIFIER_TEST # -D__DEBUG_TEST_KD_TREE

CXXFLAGS += -g -O0 #-pg
CXXFLAGS += #-static -static-libstdc++
CXXFLAGS += -lpthread
CXXFLAGS += $(DEFINES)
CXXFLAGS += -std=c++98 #-std=c++11
CXXFLAGS += -Wall -Wextra
CXXFLAGS += $(INCLUDE_DIR)
CXXFLAGS += $(shell pkg-config --cflags --libs $(PKGCONFIG_LIBS))
CXXFLAGS += -m64

LDFLAGS =

CXX = g++

CXXSOURCES = $(shell find src -name "*.cc")
OBJS = $(addprefix $(OBJ_DIR)/,$(CXXSOURCES:.cc=.o))
DEPFILES = $(OBJS:.o=.d)

.PHONY: all clean run rebuild gdb

all: $(BIN_TARGET)
	@echo "compilation succeed, issue 'make run' to execute the program."

$(OBJ_DIR)/%.o: %.cc
	@echo "[cxx] $< ..."
	@$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJ_DIR)/%.d: %.cc
	@mkdir -pv $(dir $@)
	@echo "[dep] $< ..."
	@$(CXX) $(INCLUDE_DIR) $(CXXFLAGS) -MM -MT "$(OBJ_DIR)/$(<:.cc=.o) $(OBJ_DIR)/$(<:.cc=.d)" "$<" > "$@"

sinclude $(DEPFILES)

$(BIN_DIR):
	mkdir $(BIN_DIR) -p

$(BIN_TARGET): $(BIN_DIR) $(OBJS)
	@echo "[link] $< ..."
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(CXXFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

rebuild:
	+@make clean
	+@make

run:
	#$(BIN_TARGET)
	./bin/river-and-frog

gdb: $(BIN_TARGET)
	gdb ./$(BIN_TARGET)
