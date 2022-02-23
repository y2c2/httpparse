CC=gcc

CFLAGS=-Wall -Wextra -g -O2
MODE=debug
ifeq ($(MODE),debug)
	CFLAGS=-Wall -Wextra -g -O0
else ifeq ($(MODE),release)
	CFLAGS=-Wall -Wextra -g -O2
else ifeq ($(MODE),prof)
	CFLAGS=-Wall -Wextra -g -pg -O2
endif

INCLUDES=-I../src
LDFLAGS=
RM=rm -rf
SRCS=$(wildcard *.c) $(wildcard ../src/*.c)
TARGET=test
CP=cp
RM=rm -rf

default:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LDFLAGS) 

clean:
	$(RM) $(TARGET)

install:
	$(CP) $(TARGET) /usr/bin/

uninstall:
	$(RM) /usr/bin/$(TARGET)

