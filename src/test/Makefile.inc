#
# Copyright (c) 2014-2015, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#
#     * Neither the name of Intel Corporation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
# src/test/Makefile.inc -- common Makefile defs for unit tests
#
# These defaults apply to most unit tests.  The individual Makefile
# for each unit test overrides the defaults as necessary.
#

LIBS_DIR=../..

LIBS = ../unittest/libut.a
LIBS += -L$(LIBS_DIR)/debug
LIBS += -luuid -pthread

ifeq ($(LIBPMEMBLK), y)
DYNAMIC_LIBS += -lpmemblk
STATIC_DEBUG_LIBS += $(LIBS_DIR)/debug/libpmemblk.a
STATIC_NONDEBUG_LIBS += $(LIBS_DIR)/nondebug/libpmemblk.a
endif

ifeq ($(LIBPMEMLOG), y)
DYNAMIC_LIBS += -lpmemlog
STATIC_DEBUG_LIBS += $(LIBS_DIR)/debug/libpmemlog.a
STATIC_NONDEBUG_LIBS += $(LIBS_DIR)/nondebug/libpmemlog.a
endif

ifeq ($(LIBPMEMOBJ), y)
DYNAMIC_LIBS += -lpmemobj
STATIC_DEBUG_LIBS += $(LIBS_DIR)/debug/libpmemobj.a
STATIC_NONDEBUG_LIBS += $(LIBS_DIR)/nondebug/libpmemobj.a
endif

ifeq ($(LIBPMEM),y)
DYNAMIC_LIBS += -lpmem
STATIC_DEBUG_LIBS += $(LIBS_DIR)/debug/libpmem.a
STATIC_NONDEBUG_LIBS += $(LIBS_DIR)/nondebug/libpmem.a
endif

ifeq ($(LIBVMEM),y)
DYNAMIC_LIBS += -lvmem
STATIC_DEBUG_LIBS += $(LIBS_DIR)/debug/libvmem.a
STATIC_NONDEBUG_LIBS += $(LIBS_DIR)/nondebug/libvmem.a
endif

#
# This is a helper function to be combined with usage of macros available
# in the unittest framework. It scans the code for functions that should be
# wrapped and adds required linker flags.
#
WRAPPER_FUNCS=FUNC_MOCK_RET_ALWAYS|FUNC_MOCK
PAREN=(
extract_funcs = $(shell \
    grep -Po '($(WRAPPER_FUNCS))\$(PAREN)\K[^,]*' $(1) | \
    awk '{print "-Wl,--wrap="$$0}')

INCS = -I../unittest -I../../include
CFLAGS = -ggdb -Wall -Werror -std=gnu99 $(EXTRA_CFLAGS)
LDFLAGS =

ifneq ($(TARGET),)
TARGET_STATIC_DEBUG=$(TARGET).static-debug
TARGET_STATIC_NONDEBUG=$(TARGET).static-nondebug
endif

all: $(TARGET) $(TARGET_STATIC_DEBUG) $(TARGET_STATIC_NONDEBUG)

$(TARGET_STATIC_DEBUG): $(OBJS) $(STATIC_DEBUG_LIBS) $(EXTRA_DEPS)
	$(CC) -o $@ $(OBJS) $(STATIC_DEBUG_LIBS) $(LIBS) $(LDFLAGS)

$(TARGET_STATIC_NONDEBUG): $(OBJS) $(STATIC_NONDEBUG_LIBS) $(EXTRA_DEPS)
	$(CC) -o $@ $(OBJS) $(STATIC_NONDEBUG_LIBS) $(LIBS) $(LDFLAGS)

$(TARGET): $(OBJS) $(EXTRA_DEPS)
	$(CC) -o $@ $(OBJS) $(DYNAMIC_LIBS) $(LIBS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $(INCS) $< -o $@

clean:
	$(RM) *.o */*.o core a.out *.log testfile*

clobber: clean
	$(RM) $(TARGET) $(TARGET_STATIC_DEBUG) $(TARGET_STATIC_NONDEBUG)

cstyle:
	../../../utils/cstyle -pP *.[ch]

.PHONY: all clean clobber cstyle

$(OBJS): ../unittest/unittest.h
