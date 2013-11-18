# Copyright (c) 2013 The Native Client Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#
# GNU Make based build file.  For details on GNU Make see:
# http://www.gnu.org/software/make/manual/make.html
#

# Project Build flags
WARNINGS := -Wno-long-long -Wall -Wswitch-enum -pedantic -Werror
CXXFLAGS := -pthread -std=c++11 $(WARNINGS)

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
CXXFLAGS := -I$(NACL_SDK_ROOT)/include $(CXXFLAGS) 
LDFLAGS := -L$(NACL_SDK_ROOT)/lib/pnacl/Release -lppapi_cpp -lppapi

# Declare the ALL target first, to make the 'all' target the default build
all: monte_carlo.pexe

test_mc: monte_carlo.hpp test_mc.cpp
	$(CXX) -o test_mc test_mc.cpp -std=c++11

clean:
	$(RM) monte_carlo.pexe monte_carlo.bc

monte_carlo.bc: monte_carlo.cpp monte_carlo.hpp instance_factory.hpp mc_instance.hpp
	$(PNACL_CXX) -o $@ $< -O2 $(CXXFLAGS) $(LDFLAGS)

monte_carlo.pexe: monte_carlo.bc
	$(PNACL_FINALIZE) -o $@ $<

serve:
	python -m SimpleHTTPServer 8000
