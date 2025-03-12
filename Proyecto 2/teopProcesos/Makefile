#################################
# Flags de la compilación
#################################

CC=	gcc

# Flags para debugging
CFLAGS= 	-Wall -Wextra -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function \
            -Wno-sign-conversion -Werror -fsanitize=undefined -std=gnu17 -O0 -ggdb -pthread -lrt

# Flags para optimización (descomentar para usarlas en lugar de las de debugging)
#CFLAGS=	-Wall -Wextra -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function \
#            -Wno-sign-conversion -Werror -fsanitize=undefined -std=gnu17 -O3 -fomit-frame-pointer \
#            -march=native -fno-strict-aliasing -pthread -lrt

#################################
# Compilación
#################################

PROG=		teopp

SRC=		main.c teatro.c

LIBS=

all:		$(PROG)

$(PROG):	$(SRC)
		$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $@ 

clean:
		rm -f $(PROG) *.dSYM *~

