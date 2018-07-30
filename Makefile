SRC = .
CFLAGS = -W -Wall -Ofast -march=native
LDFLAGS = 
CC = gcc

OBJ1 = o/sir.o o/read_nwk.o o/heap.o o/pcg_rnd.o

all : sir

sir: $(OBJ1)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

o/sir.o : $(SRC)/sir.c $(SRC)/sir.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/sir.c -o $@

o/read_nwk.o : $(SRC)/read_nwk.c $(SRC)/sir.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/read_nwk.c -o $@

o/heap.o : $(SRC)/heap.c $(SRC)/sir.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/heap.c -o $@

o/pcg_rnd.o : $(SRC)/pcg_rnd.c $(SRC)/sir.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/pcg_rnd.c -o $@
