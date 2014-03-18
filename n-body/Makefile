#
# $File: Makefile
# $Date: Tue Sep 10 14:43:12 2013 +0000
#
# A single output portable Makefile for
# simple c++ project

OBJ_DIR = obj
BIN_DIR = bin
TARGET = n-body-sim

BIN_TARGET = $(BIN_DIR)/$(TARGET)

DEFINES += -DENABLE_SASSERT -DENABLE_PTHREAD -DENABLE_MPI -DENABLE_OPENMP
DEFINES += -DUSE_GTK
DEFINES += -DREAL_T_LONG_DOUBLE

MPIARG ?=

DIRNAME=$(shell basename `pwd`)


PKGCONFIG_LIBS = x11 gtk+-2.0
INCLUDE_DIR = -Isrc/


CXXFLAGS += -O3 #-g -O0 #-pg
CXXFLAGS += #-static -static-libstdc++
CXXFLAGS += -lpthread -fopenmp
CXXFLAGS += $(DEFINES)
CXXFLAGS += -std=c++98 #-std=c++11
CXXFLAGS += -Wall -Wextra
CXXFLAGS += $(INCLUDE_DIR)
CXXFLAGS += $(shell pkg-config --cflags --libs $(PKGCONFIG_LIBS))
CXXFLAGS += -m64

LDFLAGS =

CXX = mpic++

CXXSOURCES = $(shell find src -name "*.cc")
OBJS = $(addprefix $(OBJ_DIR)/,$(CXXSOURCES:.cc=.o))
DEPFILES = $(OBJS:.o=.d)

.PHONY: all clean run rebuild gdb

all: $(BIN_TARGET)
	@echo "compilation succeed, issue 'make run' to execute the program."

$(OBJ_DIR)/%.o: %.cc
	@echo "[cxx] $< ..."
	@$(CXX) -c $< $(CXXFLAGS) -o $@ -g -ggdb

$(OBJ_DIR)/%.d: %.cc
	@mkdir -pv $(dir $@)
	@echo "[dep] $< ..."
	@$(CXX) $(INCLUDE_DIR) $(CXXFLAGS) -MM -MT -g -ggdb "$(OBJ_DIR)/$(<:.cc=.o) $(OBJ_DIR)/$(<:.cc=.d)" "$<" > "$@"

sinclude $(DEPFILES)

$(BIN_DIR):
	mkdir $(BIN_DIR) -p

$(BIN_TARGET): $(BIN_DIR) $(OBJS)
	@echo "[link] $< ..."
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(CXXFLAGS) -g -ggdb

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

rebuild:
	+@make clean
	+@make

run: $(BIN_TARGET)
	./$(BIN_TARGET) $(ARG)

gdb: $(BIN_TARGET)
	gdb ./$(BIN_TARGET)

remote:
	rm -rf /tmp/$(DIRNAME)
	mkdir /tmp/$(DIRNAME)
	cp -av Makefile src /tmp/$(DIRNAME)
	rm -rf /tmp/$(DIRNAME).tar.gz
	cd /tmp && tar czvf $(DIRNAME).tar.gz $(DIRNAME)
	scp /tmp/$(DIRNAME).tar.gz 2011011252@ln0:WORK
	ssh -Y 2011011252@ln0 "cd WORK && rm -rf $(DIRNAME) && tar xvf $(DIRNAME).tar.gz && rm $(DIRNAME).tar.gz && cd $(DIRNAME)"


mpi: $(BIN_TARGET)
	mpirun -np $(NWORKER) $(MPIARG) ./$(BIN_TARGET)  --parallel=mpi  $(ARG)

pthread: $(BIN_TARGET)
	./$(BIN_TARGET)  $(ARG) --parallel=pthread

openmp: $(BIN_TARGE)
	./$(BIN_TARGET)  $(ARG) --parallel=openmp


