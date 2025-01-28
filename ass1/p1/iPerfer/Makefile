## CSCI 4430 Advanced Makefile

# How to use this Makefile...
###################
###################
##               ##
##  $ make help  ##
##               ##
###################
###################

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

all: iPerfer

SOURCES = iPerfer.c
iPerfer: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o iPerfer

clean:
	rm -rf iPerfer *.dSYM

help:
	@echo "------------------ Usage ------------------"
	@echo "Build executable:           make"
	@echo "Clean build artifacts:      make clean"
	@echo "Display this help message:  make help"

.PHONY: all clean help