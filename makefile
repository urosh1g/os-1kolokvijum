CC=gcc
CFLAGS=-Wall -Wextra -O0 -g -lpthread
SRCDIRS=$(shell ls -d */)
CFILES=$(foreach D, $(SRCDIRS), $(wildcard $(D)*.c))
EXECUTABLES=$(patsubst %.c,%,$(CFILES))

all: $(EXECUTABLES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

show:
	@echo $(SRCDIRS)
	@echo $(CFILES)
	@echo $(EXECUTABLES)

clean:
	-rm -f $(EXECUTABLES)
