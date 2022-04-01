TARGET ?= labyrinth
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# SANITIZE_FLAGS := -fsanitize=undefined -fsanitize=address

# CFLAGS ?= $(INC_FLAGS) -Wall -Wextra -Wpedantic -Wno-implicit-fallthrough -std=c17 -O2
CFLAGS ?= $(INC_FLAGS) \
 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable -Wno-implicit-fallthrough \
 -Werror-implicit-function-declaration \
 $(SANITIZE_FLAGS) \
 -std=c17 -O2

LDFLAGS ?= $(SANITIZE_FLAGS)

$(TARGET): $(OBJS)
	gcc $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

all: $(TARGET)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)
	$(RM) tests/*.prog.out tests/*.prog.err

test:
	bash ./test.sh ./labyrinth tests

test2:
	bash ./test.sh ./labyrinth tests2


-include $(DEPS)
