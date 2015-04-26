# Project Build flags
WARNINGS := -Wno-long-long -Wall -Wswitch-enum -pedantic -Werror
CXXFLAGS := -pthread -std=gnu++11 $(WARNINGS)

#
# Compute tool paths
#
GETOS := python $(NACL_SDK_ROOT)/tools/getos.py
OSHELPERS = python $(NACL_SDK_ROOT)/tools/oshelpers.py
OSNAME := $(shell $(GETOS))
RM := $(OSHELPERS) rm

PNACL_TC_PATH := $(abspath $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_pnacl)
PNACL_CXX := $(PNACL_TC_PATH)/bin/pnacl-clang++
PNACL_FINALIZE := $(PNACL_TC_PATH)/bin/pnacl-finalize
PNACL_CXXFLAGS := -I$(NACL_SDK_ROOT)/include $(CXXFLAGS) 
PNACL_LDFLAGS := -L$(NACL_SDK_ROOT)/lib/pnacl/Release -lppapi_simple -lnacl_io -lppapi_cpp -lppapi -lpthread 

IMPROC_HEADERS := simple_template.cpp 

# Declare the ALL target first, to make the 'all' target the default build
all: simple_template.pexe


clean:
	$(RM) simple_template.pexe simple_template.bc

#$(PROC_OBJECTS): $(PROCESSORS) singleton_factory.hpp
#	$(PNACL_CXX) -o $@ $< -O2 $(CXXFLAGS) $(LDFLAGS)

simple_template.bc:  $(IMPROC_HEADERS) $(PROCESSORS) 
	$(PNACL_CXX) -o $@ $< -O2 $(PNACL_CXXFLAGS) $(PNACL_LDFLAGS)

simple_template.pexe: simple_template.bc
	$(PNACL_FINALIZE) -o $@ $<

serve:
	python -m SimpleHTTPServer 8000
