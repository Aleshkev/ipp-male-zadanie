TARGET ?= labyrinth
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# CFLAGS ?= $(INC_FLAGS) -Wall -Wextra -Wpedantic -Wno-implicit-fallthrough -std=c17 -O2
CFLAGS ?= $(INC_FLAGS) -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable -Wno-implicit-fallthrough -std=c17 -O2

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

all: $(TARGET)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)
	$(RM) tests/*.prog.out tests/*.prog.err

test:
	bash ./test.sh ./labyrinth tests

-include $(DEPS)
