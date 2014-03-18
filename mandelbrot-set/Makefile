#
# $File: Makefile
# $Date: Tue Sep 03 22:21:38 2013 +0000
#
# A single output portable Makefile for
# simple c++ project

OBJ_DIR = obj
BIN_DIR = bin
TARGET = mandelbrot-set

BIN_TARGET = $(BIN_DIR)/$(TARGET)

DEFINES += -DUSE_GTK -DENABLE_SASSERT -DENABLE_PTHREAD -DENABLE_MPI -DENABLE_OPENMP
DEFINES += -DREAL_T_LONG_DOUBLE

DOMAIN ?= -2.27777777777777777,-1,3.5555555555555555,2
SIZE ?= 1280x720
PMETHOD ?= pthread
NWORKER ?= 4
NITER ?= 2048

MPIARG ?=
PARAM ?= --niter=$(NITER) --domain=$(DOMAIN) --size=$(SIZE) --parallel=$(PMETHOD)

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

run: $(BIN_TARGET)
	./$(BIN_TARGET) $(PARAM) $(ARG) --show=rgb

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
	mpirun -np $(NWORKER) $(MPIARG) ./$(BIN_TARGET) $(PARAM) --parallel=mpi --show=rgb $(ARG)

pthread: $(BIN_TARGET)
	./$(BIN_TARGET) $(PARAM) $(ARG) --parallel=pthread --show=rgb

openmp: $(BIN_TARGE)
	./$(BIN_TARGET) $(PARAM) $(ARG) --parallel=openmp --show=rgb


