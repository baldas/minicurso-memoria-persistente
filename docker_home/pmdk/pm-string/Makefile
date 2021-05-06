# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2015-2016, Intel Corporation

#
#

PROGS = pm-stat-str1 pm-stat-str2 pm-stat-str3 pm-stat-str4 \
				pm-dyn-str1 pm-dyn-str2

LIBS = -lpmemobj -lpmem -pthread

LINKER=$(CC)
ifeq ($(COMPILE_LANG), cpp)
LINKER=$(CXX)
endif


MAKEFILE_DEPS=Makefile

all: $(PROGS)

%.o: %.c $(MAKEFILE_DEPS)
	$(call check-cstyle, $<)
	$(CC) -c -o $@ $(CFLAGS) $(INCS) $<

%.o: %.cpp $(MAKEFILE_DEPS)
	$(call check-cstyle, $<)
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCS) $<

$(PROGS):
	$(LINKER) -o $@ $^ $(LDFLAGS) $(LIBS)


pm-stat-str1: pm-stat-str1.o
pm-stat-str2: pm-stat-str2.o
pm-stat-str3: pm-stat-str3.o
pm-stat-str4: pm-stat-str4.o

pm-dyn-str1: pm-dyn-str1.o
pm-dyn-str2: pm-dyn-str2.o

clean:
	rm -f $(PROGS)
	rm -f *.o
