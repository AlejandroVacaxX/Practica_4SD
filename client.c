#include <stdio.h>
#include <string.h>
#include <atmi.h>
#include <fml32.h>
#include "variables.flds.h"

// funciones modulares para llenar el buffer de cada operacion
void empaquetar_datos_insert(FBFR32 *sendbuf);
void empaquetar_datos_select(FBFR32 *sendbuf);
void empaquetar_datos_update(FBFR32 *sendbuf);
void empaquetar_datos_delete(FBFR32 *sendbuf);

int main(int argc, char *argv[]) {
    FBFR32 *sendbuf;
    long rcvlen;

    printf("🦈🦈 de Operaciones Distribuidas - Tiburones 🦈🦈\n");

    if (tpinit((TPINIT *) NULL) == -1) {
        printf("Error en tpinit, tperrno = %d\n", tperrno);
        return 1;
    }

    sendbuf = (FBFR32 *)tpalloc("FML32", NULL, 2048);
    if (sendbuf == NULL) {
        printf("Error asignando buffer FML32\n");
        tpterm();
        return 1;
    }

    // probando creacion de registro
    printf("\n--- iniciando insert ---\n");
    Finit32(sendbuf, Fsizeof32(sendbuf));
    empaquetar_datos_insert(sendbuf);
    if (tpcall("INSERT_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
        printf("Error en tpcall insert: tperrno = %d\n", tperrno);
    } else {
        printf("Operacion INSERT exitosa.\n");
    }

   // probando lectura de registro
   printf("\n--- iniciando select ---\n");
   Finit32(sendbuf, Fsizeof32(sendbuf));
   empaquetar_datos_select(sendbuf);
   
   // extraer y mostrar lo que vamos a buscar
    // inicializamos en cero para evitar basura de memoria en la impresion


   char nombre_busqueda[50];
   Fget32(sendbuf, NOMBRE, 0, nombre_busqueda, 0);
   printf("query select -> buscando al tiburon: %s\n", nombre_busqueda);

   if (tpcall("SELECT_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
       printf("error en tpcall select: tperrno = %d\n", tperrno);
   } else {
       printf("operacion select exitosa.\n");
       
       // extraer y mostrar los datos que el servidor nos devolvio
       // inicializamos en cero para evitar basura de memoria en la impresion
       char genero_recuperado[3] = {0};
       char etapa_recuperada[50] = {0};
       
       Fget32(sendbuf, GENERO, 0, genero_recuperado, 0);
       Fget32(sendbuf, ETAPA, 0, etapa_recuperada, 0);
       
       printf("resultado select -> genero: %s, etapa: %s\n", genero_recuperado, etapa_recuperada);
   }

   // probando actualizacion de registro
   printf("\n--- iniciando update ---\n");
   Finit32(sendbuf, Fsizeof32(sendbuf));
   empaquetar_datos_update(sendbuf);
   
   // extraer y mostrar los datos de la peticion update
   char nombre_update[50];
   char genero_nuevo[3];
   Fget32(sendbuf, NOMBRE, 0, nombre_update, 0);
   Fget32(sendbuf, GENERO, 0, genero_nuevo, 0);
   printf("query update -> registro a modificar: %s\n", nombre_update);
   printf("query update -> nuevo valor de genero: %s\n", genero_nuevo);

   if (tpcall("UPDATE_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
       printf("error en tpcall update: tperrno = %d\n", tperrno);
   } else {
       printf("operacion update exitosa.\n");
       
       // extraer y confirmar como quedo el buffer tras la modificacion
       char genero_confirmado[3];
       Fget32(sendbuf, GENERO, 0, genero_confirmado, 0);
       
       printf("resultado update -> el registro %s ahora tiene genero %s\n", nombre_update, genero_confirmado);
   }

    // probando eliminacion de registro
    printf("\n--- iniciando delete ---\n");
    Finit32(sendbuf, Fsizeof32(sendbuf));
    empaquetar_datos_delete(sendbuf);
    if (tpcall("DELETE_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
        printf("Error en tpcall delete: tperrno = %d\n", tperrno);
    } else {
        printf("Operacion DELETE exitosa.\n");
    }

    tpfree((char *)sendbuf);
    tpterm();
    return 0;
}

void empaquetar_datos_insert(FBFR32 *sendbuf) {
    char nombre[50] = "Mandibulas";
    char genero[3] = "M";
    char etapa[50] = "Adult";
    long pies = 14;
    long pulgadas = 5;
    char descripcion[100] = "Avistado en costas del Pacifico.";
    long id_especie = 1;

    Fadd32(sendbuf, NOMBRE, nombre, 0);
    Fadd32(sendbuf, GENERO, genero, 0);
    Fadd32(sendbuf, ETAPA, etapa, 0);
    Fadd32(sendbuf, PIES, (char *)&pies, 0);
    Fadd32(sendbuf, PULGADAS, (char *)&pulgadas, 0);
    Fadd32(sendbuf, DESCRIPCION, descripcion, 0);
    Fadd32(sendbuf, ID_ESPECIE, (char *)&id_especie, 0);
}

void empaquetar_datos_select(FBFR32 *sendbuf) {
    char nombre[50] = "Mandibulas";
    Fadd32(sendbuf, NOMBRE, nombre, 0);
}

void empaquetar_datos_update(FBFR32 *sendbuf) {
    char nombre[50] = "Mandibulas";
    char genero[3] = "F";
    
    Fadd32(sendbuf, NOMBRE, nombre, 0);
    Fadd32(sendbuf, GENERO, genero, 0);
}

void empaquetar_datos_delete(FBFR32 *sendbuf) {
    long id_especie = 1;
    Fadd32(sendbuf, ID_ESPECIE, (char *)&id_especie, 0);
}