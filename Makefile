MAKE=make
AR=ar
AR_FLAGS=rcs
CP=cp
RM=rm -rf
MKDIR=mkdir -p
TARGET=libhttpparse.a
SRCDIR=./src
SRCS=$(wildcard ./src/*.c)
HDRS=$(wildcard ./src/*.h)
OBJS=$(patsubst ./src/%.c,./src/.build/%.o,$(wildcard ./src/*.c))

ifeq ($(OS),Windows_NT)
else
    UNAME_S:=$(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        AR=libtool
        AR_FLAGS=-static -o
    endif
endif
 
debug: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	@for entry in $(SRCDIR); do \
		$(MAKE) -C $$entry; \
	done
	$(AR) $(AR_FLAGS) $(TARGET) $(OBJS)

clean:
	@for entry in $(SRCDIR); do \
		$(MAKE) -C $$entry clean; \
	done
	$(RM) $(TARGET)

install:
	$(CP) $(TARGET) /usr/lib/
	$(MKDIR) /usr/include/libhttpparse/
	for entry in $(HDRS); do \
		$(CP) $$entry /usr/include/libhttpparse; \
	done

uninstall:
	$(RM) /usr/lib/$(TARGET)
	$(RM) /usr/include/qvhttp/


