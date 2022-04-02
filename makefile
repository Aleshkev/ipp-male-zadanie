# Welcome to the makefile file.
# `make` will make in release mode, `make DEBUG=yes` will make in debug mode (for valgrind).
# Use `make -B ...` if you're switching between these modes.

SRCS := $(wildcard ./src/*.c)
OBJS := $(addsuffix .o, $(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find ./src -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC := gcc

CFLAGS += $(INC_FLAGS) \
 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable -Wno-implicit-fallthrough \
 -Werror-implicit-function-declaration
 -std=c17


ifdef DEBUG
CFLAGS += -O0 -g
else
CFLAGS += -O2
endif

# DO NOT USE WITH VALGRIND
ifdef SANITIZE
SANITIZE_FLAGS := -fsanitize=undefined -fsanitize=address
CFLAGS += $(SANITIZE_FLAGS)
LDFLAGS += $(SANITIZE_FLAGS)
endif

TARGET := labyrinth
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

all: $(TARGET)

.PHONY: clean
clean:
	@ $(RM) src/*.o
	@ $(RM) $(TARGET) $(OBJS) $(DEPS)
	@ $(RM) tests/*.prog.out tests/*.prog.err

test:
	bash ./test.sh ./labyrinth tests

# Helpful configurations for testing & debugging.
make_release: clean
	make -B all
make_debug: clean
	make -B DEBUG=yes all
test_valid: make_release
	python3 ./test.py -e.
test_invalid: make_release
	python3 ./test.py -e12345 --standardize
test_memory: make_debug
	python3 ./test.py -e.12345 --valgrind --standardize
test_all: make_release
	python3 ./test.py -e.012345
test_error_0: make_release
	python3 ./test.py -e0 --timeout 5

# Prepare for submitting to Moodle
prepare: clean
	tar -czvf ipp-male-zadanie.tar.gz \
		src/*.c src/*.h makefile test.py test.sh

-include $(DEPS)
