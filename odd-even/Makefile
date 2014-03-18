#
# $File: Makefile
# $Date: Fri Aug 23 09:20:08 2013 +0000
#
# A single output portable Makefile for
# simple c++ project

OBJ_DIR = obj
BIN_DIR = bin
TARGET = odd-even-sort

BIN_TARGET = $(BIN_DIR)/$(TARGET)

NN ?= 100000
NP ?= 4

PKGCONFIG_LIBS =
INCLUDE_DIR =

DEFINES = #-D__DEBUG_BUILD  #-D__SIMPLIFIER_TEST # -D__DEBUG_TEST_KD_TREE

CXXFLAGS += -O3 #-g -O0 #-pg
CXXFLAGS += #-static -static-libstdc++
CXXFLAGS += $(DEFINES)
CXXFLAGS += #-std=c++11
CXXFLAGS += -Wall -Wextra
CXXFLAGS += $(INCLUDE_DIR)
CXXFLAGS += $(PKGCONFIG_LIBS)
CXXFLAGS += -m64

LDFLAGS =


CXX = mpic++

CXXSOURCES = $(shell find src -name "*.cc")
OBJS = $(addprefix $(OBJ_DIR)/,$(CXXSOURCES:.cc=.o))
DEPFILES = $(OBJS:.o=.d)

.PHONY: all clean run rebuild gdb

all: $(BIN_TARGET)
	@mkdir -p run/log
	@rm -f run/$(TARGET)
	@bash -c "cd run && ln -s ../$(BIN_TARGET) ."
	@echo "compilation succeed, enter directory 'run' for executing the program."

$(OBJ_DIR)/%.o: %.cc
	@echo "[cxx] $< ..."
	@$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJ_DIR)/%.d: %.cc
	@mkdir -pv $(dir $@)
	@echo "[dep] $< ..."
	@$(CXX) $(INCLUDE_DIR) $(CXXFLAGS) -MM -MT "$(OBJ_DIR)/$(<:.cc=.o) $(OBJ_DIR)/$(<:.cc=.d)" "$<" > "$@"

sinclude $(DEPFILES)

$(BIN_TARGET): $(OBJS)
	@echo "[link] $< ..."
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(CXXFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

rebuild:
	+@make clean
	+@make

run: $(BIN_TARGET)
	cd run && mpirun -np $(NP) ./$(TARGET) $(NN)

utils:
	g++ ./mkdata.cpp -o mkdata -O3
	g++ ./check.cpp -o check -O3

data:
	@rm sort.in -f
	@./mkdata $(NN) > sort.in

dist:
	@for host in `cat hostfile | grep -v -E '^#' | grep -v '127.0.0.1' | awk '{print $$1}'`; do \
		echo $$host; \
		scp ./odd-even-sort $$host: ;\
	done;

sub:
	rm -rf /tmp/oes-pak
	mkdir /tmp/oes-pak
	cp -av Makefile src check.cpp /tmp/oes-pak
	cd /tmp && tar czvf oes-pak.tar.gz oes-pak
	scp /tmp/oes-pak.tar.gz 2011011253@ln0:
	ssh 2011011253@ln0 "rm oes-pak -rf && tar xvf oes-pak.tar.gz && rm -rf oes-pak.tar.gz && cd oes-pak && make && g++ check.cpp -o check -O3 && cp check bin/odd-even-sort ../WORK"

gdb: $(BIN_TARGET)
	gdb ./$(BIN_TARGET)
