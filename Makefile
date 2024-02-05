CC=gcc
CFLAGS=-std=gnu17 -Wall -fsanitize=address,undefined
LDFLAGS=-fsanitize=address,undefined
LDLIBS=-lpthread -lm

SRCS=src/spell_checker.c src/dictionary.c src/error_handler.c
HDRS=src/spell_checker.h src/dictionary.h src/error_handler.h

TARGET=build/spell_checker

all: ${TARGET}

${TARGET}: ${SRCS} ${HRDS}
	${CC} ${CFLAGS} -o ${TARGET} ${SRCS} ${LDFLAGS} ${LDLIBS}	

.PHONY: clean all

clean:
	-rm -f ${TARGET}