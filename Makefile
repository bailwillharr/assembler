BIN	:= assembler
BINDIR	:= bin
SRCDIR	:= src

CC	:= gcc
CFLAGS	:= -MMD -std=c99 -Wall -g
LDFLAGS	:=

src	:= $(wildcard $(SRCDIR)/*.c)
obj	:= $(src:.c=.o)
dep	:= $(obj:.o=.d)

$(BINDIR)/$(BIN): $(obj)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

-include $(dep)

.PHONY: clean
clean:
	rm -rf $(BINDIR)
	rm -f $(SRCDIR)/*.o
	rm -f $(SRCDIR)/*.d
