SYSTEM= x86-64_Linux
LIBFORMAT=static_pic
CC=gcc
CPLEXDIR=/users/ecco/ptigwe/local/ibm/ILOG/CPLEX_Studio126/cplex
CPLEXINC=$(CPLEXDIR)/include
CPLEXLIB=$(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)/
CFLAGS=-g -I$(CPLEXINC) -Wuninitialized -Wall
CPLEXLDFLAGS=-lcplex -lpthread
LDDIR=-L$(CPLEXLIB) -L$(HOME)/local/lib
LDFLAGS=$(CPLEXLDFLAGS) -lgmp -lm -lrt -lpthread

OBJ= cplp.o matrix.o io.o util.o polymatrix.o strategy.o

%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
	
all: $(OBJ)

clean:
	rm $(OBJ)
