BIN	:= assembler
BINDIR	:= bin
SRCDIR	:= src

CC	:= gcc
CFLAGS	:= -MMD -std=c99 -Wall
LDFLAGS	:=

src	:= $(wildcard $(SRCDIR)/*.c)
obj	:= $(src:.c=.o)
dep	:= $(obj:.o=.d)

$(BIN): $(obj)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINDIR)/$@ $^

-include $(dep)

.PHONY: clean
clean:
	rm -rf $(BINDIR)
	rm -f $(SRCDIR)/*.o
	rm -f $(SRCDIR)/*.d

.PHONY: run
run: $(BIN)
	./$(BINDIR)/$(BIN)
