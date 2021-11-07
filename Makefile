BIN		:= assembler
LIBS	:=
SRCDIR	:= src
CC		:= gcc
CFLAGS	:= -MMD -std=c99 -Wall -Og -g -DDEBUG
LDFLAGS	:= $(LIBS)

src	:= $(shell find $(SRCDIR) -name "*.c")
obj	:= $(src:.c=.o)
dep	:= $(obj:.o=.d)

all: $(BIN)

$(BIN): $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(dep)

clean:
	rm -f $(BIN)
	rm -f $(obj) $(dep)

.PHONY: all clean
