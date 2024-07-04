PROJ=crun
SRCS=first_round.c ../aes_source/aes_util.c ../aes_source/aes.c
OBJS=${SRCS:.c=.o}

CFLAGS=-g -O3
LDFLAGS=-g
LDLIBS=-static -lmastik


all: ${PROJ}

${PROJ}:${OBJS}
	cc ${LDFLAGS} -o ${PROJ} ${OBJS} ${LDLIBS}

clean:
	rm -f ${PROJ} ${OBJS}
