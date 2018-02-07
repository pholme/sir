SRC = .
SFMT_SRC = ../SFMT
CFLAGS = -W -Wall -DSFMT_MEXP=19937 -Ofast -DSFMT_HAVE_SSE2=1 -march=native
LDFLAGS = #-g
CC = gcc

OBJ1 = o/sir.o o/SFMT.o o/misc.o o/heap.o

all : sir

sir: $(OBJ1)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

o/SFMT.o : $(SFMT_SRC)/SFMT.c $(SFMT_SRC)/SFMT.h $(SFMT_SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SFMT_SRC)/SFMT.c -o $@

o/sir.o : $(SRC)/sir.c $(SRC)/sir.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/sir.c -o $@

o/misc.o : $(SRC)/misc.c $(SRC)/sir.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/misc.c -o $@

o/heap.o : $(SRC)/heap.c $(SRC)/sir.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/heap.c -o $@

