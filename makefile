CFLAGS = -O -I$(TUXDIR)/include -I$(ORACLE_HOME)/precomp/public -I$(ORACLE_HOME)/rdbms/public
ORACLE_LIBS = -L$(ORACLE_HOME)/lib -lclntsh

all: campos server client

campos: variables.flds
	mkfldhdr32 variables.flds

server.c: server.pc campos
	proc iname=server.pc oname=server.c parse=none

server: server.c
	buildserver -o server -f "-I$(ORACLE_HOME)/precomp/public" -f "-I$(ORACLE_HOME)/rdbms/public" -f server.c -f "$(ORACLE_LIBS)" -s INSERT_FML32

client: client.c campos
	buildclient -o client -f client.c

clean:
	rm -f server client server.c server.lis variables.flds.h tuxconfig*
