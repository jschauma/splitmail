PROG=	splitmbox
OFILES=	splitmbox.o
PREFIX?=/usr/local

CFLAGS+=	-O3 -Wall -Werror -Wextra #-g

all: ${PROG}

${PROG}: ${OFILES}
	${CC} ${CFLAGS} -o ${PROG} ${OFILES}

install: ${PROG}
	mkdir -p ${PREFIX}/bin ${PREFIX}/man/man1
	install -c -m 755 ${PROG} ${PREFIX}/bin/${PROG}
	install -c -m 644 ${PROG}.1 ${PREFIX}/man/man1/${PROG}.1

clean:
	rm -f ${OFILES} ${PROG}
