PROJ=crun
SRCS=main.c aes_source/aes_util.c aes_source/aes.c
OBJS=${SRCS:.c=.o}

CFLAGS=-g -DESTDEF=$(EST) 
LDFLAGS=-g
LDLIBS=-static -lmastik


all: ${PROJ}

${PROJ}:${OBJS}
	cc ${CFLAGS} ${LDFLAGS} -o ${PROJ} ${OBJS} ${LDLIBS}

clean:
	rm -f ${PROJ} ${OBJS}
