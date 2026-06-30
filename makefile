CFLAGS = -O -I$(TUXDIR)/include -I$(ORACLE_HOME)/precomp/public -I$(ORACLE_HOME)/rdbms/public
ORACLE_LIBS = -L$(ORACLE_HOME)/lib -lclntsh

# Lista de fuentes Pro*C de la arquitectura
PC_SOURCES = server.pc insert.pc select.pc
C_GEN_FILES = server.c insert.c select.c
O_FILES = insert.o select.o

all: campos server client

campos: variables.flds
	mkfldhdr32 variables.flds

# Regla generica para precompilar cualquier archivo .pc a .c con Oracle proC
%.c: %.pc campos
	proc iname=$< oname=$@ parse=none 

# Compilar los modulos auxiliares a archivos objeto (.o)
%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

# Buildserver une el nucleo de tuxedo con todos los modulos compilados y las banderas de Oracle
server: $(C_GEN_FILES) $(O_FILES)
	buildserver -o server -f "-I$(ORACLE_HOME)/precomp/public" -f "-I$(ORACLE_HOME)/rdbms/public" \
	-f "server.c $(O_FILES)" -f "$(ORACLE_LIBS)" \
	-s INSERT_FML32 -s SELECT_FML32

client: client.c campos
	buildclient -o client -f client.c

clean:
	rm -f server client $(C_GEN_FILES) *.o *.lis variables.flds.h tuxconfig*


