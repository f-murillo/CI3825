#################################
# Flags de la compilación
#################################

CC=	gcc

# Flags para debbugging
CFLAGS= 	-Wall -Wextra -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Werror -fsanitize=undefined -std=gnu17 -O0 -ggdb 


# Flags para optimización
#CFLAGS=	-Wall -Wextra -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Werror -fsanitize=undefined -std=gnu17 -O3 -fomit-frame-pointer -march=native -fno-strict-aliasing 
#DFLAGS=			

#################################
# Compilación
#################################

PROG=		simfs

SRC=		 simfs.c filesystem.c node.c

LIBS=

all:		$(PROG)

$(PROG):	$(SRC)
		$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $@ 

clean:
		rm -f $(PROG) *.dSYM *~
