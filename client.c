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
    int opcion = 0;
    
    // variables para recuperar y mostrar datos de los buffers
    char nombre_busqueda[50];
    char genero_recuperado[3];
    char etapa_recuperada[50];
    char nombre_update[50];
    char genero_nuevo[3];
    char genero_confirmado[3];
    
    // variables adicionales para mostrar todo lo recuperado en el select
    long pies_recuperado = 0;
    long pulgadas_recuperado = 0;
    char descripcion_recuperada[100];
    long id_especie_recuperado = 0;

    printf("sistema de operaciones distribuidas - tiburones\n");

    if (tpinit((TPINIT *) NULL) == -1) {
        printf("error en tpinit, tperrno = %d\n", tperrno);
        return 1;
    }

    sendbuf = (FBFR32 *)tpalloc("FML32", NULL, 2048);
    if (sendbuf == NULL) {
        printf("error asignando buffer fml32\n");
        tpterm();
        return 1;
    }

    // ciclo principal del menu
    do {
        printf("\n--- menu de operaciones ---\n");
        printf("1 select\n");
        printf("2 insert\n");
        printf("3 update\n");
        printf("4 delete\n");
        printf("5 salir\n");
        printf("seleccione una opcion: ");
        
        // capturar entrada y limpiar buffer en caso de error
        if (scanf("%d", &opcion) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (opcion >= 1 && opcion <= 4) {
            Finit32(sendbuf, Fsizeof32(sendbuf));
        }

        switch(opcion) {
            case 1:
                // probando lectura de registro
                printf("\n--- iniciando select ---\n");
                empaquetar_datos_select(sendbuf);
                
                // inicializamos en cero para evitar basura de memoria en la impresion
                memset(nombre_busqueda, 0, sizeof(nombre_busqueda));
                Fget32(sendbuf, NOMBRE, 0, nombre_busqueda, 0);
                printf("query select -> buscando al tiburon: %s\n", nombre_busqueda);

                if (tpcall("SELECT_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
                    printf("error en tpcall select: tperrno = %d\n", tperrno);
                } else {
                    printf("operacion select exitosa.\n");
                    
                    memset(genero_recuperado, 0, sizeof(genero_recuperado));
                    memset(etapa_recuperada, 0, sizeof(etapa_recuperada));
                    memset(descripcion_recuperada, 0, sizeof(descripcion_recuperada));
                    pies_recuperado = 0;
                    pulgadas_recuperado = 0;
                    id_especie_recuperado = 0;
                    
                    Fget32(sendbuf, GENERO, 0, genero_recuperado, 0);
                    Fget32(sendbuf, ETAPA, 0, etapa_recuperada, 0);
                    Fget32(sendbuf, PIES, 0, (char *)&pies_recuperado, 0);
                    Fget32(sendbuf, PULGADAS, 0, (char *)&pulgadas_recuperado, 0);
                    Fget32(sendbuf, DESCRIPCION, 0, descripcion_recuperada, 0);
                    Fget32(sendbuf, ID_ESPECIE, 0, (char *)&id_especie_recuperado, 0);
                    
                    printf("resultado select -> genero: %s, etapa: %s\n", genero_recuperado, etapa_recuperada);
                    printf("                    tamano: %ld pies, %ld pulgadas\n", pies_recuperado, pulgadas_recuperado);
                    printf("                    descripcion: %s\n", descripcion_recuperada);
                    printf("                    id especie: %ld\n", id_especie_recuperado);
                }
                break;

            case 2:
                // probando creacion de registro
                printf("\n--- iniciando insert ---\n");
                empaquetar_datos_insert(sendbuf);
                
                if (tpcall("INSERT_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
                    printf("error en tpcall insert: tperrno = %d\n", tperrno);
                } else {
                    printf("operacion insert exitosa.\n");
                }
                break;

            case 3:
                // probando actualizacion de registro
                printf("\n--- iniciando update ---\n");
                empaquetar_datos_update(sendbuf);
                
                memset(nombre_update, 0, sizeof(nombre_update));
                memset(genero_nuevo, 0, sizeof(genero_nuevo));
                
                Fget32(sendbuf, NOMBRE, 0, nombre_update, 0);
                Fget32(sendbuf, GENERO, 0, genero_nuevo, 0);
                
                printf("query update -> registro a modificar: %s\n", nombre_update);
                printf("query update -> nuevo valor de genero: %s\n", genero_nuevo);

                if (tpcall("UPDATE_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
                    printf("error en tpcall update: tperrno = %d\n", tperrno);
                } else {
                    printf("operacion update exitosa.\n");
                    
                    memset(genero_confirmado, 0, sizeof(genero_confirmado));
                    Fget32(sendbuf, GENERO, 0, genero_confirmado, 0);
                    
                    printf("resultado update -> el registro %s ahora tiene genero %s\n", nombre_update, genero_confirmado);
                }
                break;

            case 4:
                // probando eliminacion de registro
                printf("\n--- iniciando delete ---\n");
                empaquetar_datos_delete(sendbuf);
                
                if (tpcall("DELETE_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
                    printf("error en tpcall delete: tperrno = %d\n", tperrno);
                } else {
                    printf("operacion delete exitosa.\n");
                }
                break;
                
            case 5:
                // finaliza la ejecucion
                printf("cerrando sistema...\n");
                break;
                
            default:
                // captura de valor fuera de rango
                printf("opcion no valida, intente nuevamente.\n");
                break;
        }
    } while(opcion != 5);

    tpfree((char *)sendbuf);
    tpterm();
    return 0;
}

void empaquetar_datos_insert(FBFR32 *sendbuf) {
    char nombre[50];
    char genero[3];
    char etapa[50];
    long pies;
    long pulgadas;
    char descripcion[100];
    long id_especie;

    // lectura de cada atributo desde el teclado
    printf("ingrese el nombre: ");
    scanf(" %[^\n]", nombre);
    printf("ingrese el genero: ");
    scanf(" %[^\n]", genero);
    printf("ingrese la etapa: ");
    scanf(" %[^\n]", etapa);
    printf("ingrese tamano en pies: ");
    scanf("%ld", &pies);
    printf("ingrese tamano en pulgadas: ");
    scanf("%ld", &pulgadas);
    printf("ingrese una descripcion: ");
    scanf(" %[^\n]", descripcion);
    printf("ingrese el id de la especie: ");
    scanf("%ld", &id_especie);

    Fadd32(sendbuf, NOMBRE, nombre, 0);
    Fadd32(sendbuf, GENERO, genero, 0);
    Fadd32(sendbuf, ETAPA, etapa, 0);
    Fadd32(sendbuf, PIES, (char *)&pies, 0);
    Fadd32(sendbuf, PULGADAS, (char *)&pulgadas, 0);
    Fadd32(sendbuf, DESCRIPCION, descripcion, 0);
    Fadd32(sendbuf, ID_ESPECIE, (char *)&id_especie, 0);
}

void empaquetar_datos_select(FBFR32 *sendbuf) {
    char nombre[50];
    
    // parametro de busqueda definido por el usuario
    printf("ingrese el nombre del registro a buscar: ");
    scanf(" %[^\n]", nombre);
    
    Fadd32(sendbuf, NOMBRE, nombre, 0);
}

void empaquetar_datos_update(FBFR32 *sendbuf) {
    char nombre[50];
    char genero[3];
    
    // parametros de modificacion ingresados manualmente
    printf("ingrese el nombre a modificar: ");
    scanf(" %[^\n]", nombre);
    printf("ingrese el nuevo genero: ");
    scanf(" %[^\n]", genero);
    
    Fadd32(sendbuf, NOMBRE, nombre, 0);
    Fadd32(sendbuf, GENERO, genero, 0);
}

void empaquetar_datos_delete(FBFR32 *sendbuf) {
    long id_especie;
    
    // identificador capturado por teclado para borrar
    printf("ingrese el id de la especie a eliminar: ");
    scanf("%ld", &id_especie);
    
    Fadd32(sendbuf, ID_ESPECIE, (char *)&id_especie, 0);
}