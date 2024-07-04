PROJ=crun
SRCS=generate_key.c ../../../aes_source/aes_util.c ../../../aes_source/aes.c
OBJS=${SRCS:.c=.o}

CFLAGS=-g
LDFLAGS=-g
LDLIBS=-static -lmastik


all: ${PROJ}

${PROJ}:${OBJS}
	cc ${LDFLAGS} -o ${PROJ} ${OBJS} ${LDLIBS}

clean:
	rm -f ${PROJ} ${OBJS}
