#include <stdio.h>
#include <string.h>
#include <atmi.h>
#include <fml32.h>
#include "variables.flds.h"

int main(int argc, char *argv[]) {
    FBFR32 *sendbuf;
    long rcvlen;

    printf("Select Ejecutandose");
    printf("🦈🦈🦈 CAPTURA DE DATOS DEL EJEMPLAR 🦈🦈🦈\n");
    printf("Conectando con la aplicacion Tuxedo...\n");

    if (tpinit((TPINIT *) NULL) == -1) {
        printf("Error en la conexion (tpinit), tperrno = %d\n", tperrno);
        return 1;
    }

    sendbuf = (FBFR32 *)tpalloc("FML32", NULL, 1024);
    if (sendbuf == NULL) {
        printf("Error asignando buffer FML32\n");
        tpterm();
        return 1;
    }

    /* Datos quemados para prueba rapida */
    char nombre[50] = "Mandibulas";
    char genero[3] = "M";
    char etapa[50] = "Adult";
    long pies = 14;
    long pulgadas = 5;
    char descripcion[100] = "Tiburon blanco avistado cerca de la costa de Acapulco.";
    char fecha_tag[50] = "2026-06-26 12:00:00";
    char lugar_tag[50] = "Acapulco";
    long id_especie = 1;

    Fadd32(sendbuf, NOMBRE, nombre, 0);
    Fadd32(sendbuf, GENERO, genero, 0);
    Fadd32(sendbuf, ETAPA, etapa, 0);
    Fadd32(sendbuf, PIES, (char *)&pies, 0);
    Fadd32(sendbuf, PULGADAS, (char *)&pulgadas, 0);
    Fadd32(sendbuf, DESCRIPCION, descripcion, 0);
    Fadd32(sendbuf, FECHA_TAG, fecha_tag, 0);
    Fadd32(sendbuf, LUGAR_TAG, lugar_tag, 0);
    Fadd32(sendbuf, ID_ESPECIE, (char *)&id_especie, 0);

    printf("Invocando el servicio 'INSERT_FML32'...\n");
    if (tpcall("INSERT_FML32", (char *)sendbuf, 0, (char **)&sendbuf, &rcvlen, 0) == -1) {
        printf("Error en la llamada al servicio: tperrno = %d\n", tperrno);
    } else {
        printf("¡Tiburón registrado exitosamente en Oracle!\n");
    }

    tpfree((char *)sendbuf);
    tpterm();
    return 0;
}
