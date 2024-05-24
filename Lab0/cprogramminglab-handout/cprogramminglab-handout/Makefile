# -*- Makefile -*- for cprogramminglab (handout edition)

CC = clang
CFLAGS = -std=c11 -Og -g -Werror -Wall -Wextra -Wpedantic -Wconversion
CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wwrite-strings
CFLAGS += -Wno-unused-parameter -fsanitize=address,undefined
LDFLAGS = -fsanitize=address,undefined

PROGRAMS = qtest
all: $(PROGRAMS)

# Linking rules
qtest: qtest.o report.o console.o harness.o queue.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Header dependencies
console.o: console.c console.h report.h
harness.o: harness.c harness.h report.h
qtest.o: qtest.c console.h harness.h queue.h report.h
queue.o: queue.c harness.h queue.h
report.o: report.c report.h

# Tests
check: qtest driver.py
	./driver.py

clean:
	rm -f *.o $(PROGRAMS)

.PHONY: all check clean

# Code formatting and submission is handled by helper.mk.
HANDOUT_SCRIPTS = driver.py
HANDIN_TAR = cprogramminglab-handin.tar
HANDIN_FILES = queue.c queue.h .clang-format .format-checked
FORMAT_FILES = queue.c queue.h
include helper.mk
