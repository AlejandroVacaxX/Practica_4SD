#include <stdio.h>
#include <string.h>
#include <atmi.h>
#include <fml32.h>
#include <userlog.h>
#include "variables.flds.h"


void SELECT_FML32(TPSVCINFO *rqst){
    FBFR32 *transb;
    paramConexion svL_paramCon;

    EXEC SQL BEGIN DECLARE SECTION;
        char h_nombre[50];
        char h_genero[3];
        char h_etapa[50];
        long h_pies;
        long h_pulgadas;
        char h_descripcion[100];
        char h_fecha_tag[50];
        char h_lugar_tag[50];
        long h_id_especie;
    EXEC SQL END DECLARE SECTION;

    transb = (FBFR32 *)rqst->data;

    // extraer el nombre que el cliente quiere buscar
    if (Fget32(transb, NOMBRE, 0, h_nombre, 0) < 0) {
        userlog("Error: No se recibio el campo NOMBRE en el buffer FML32");
        tpreturn(TPFAIL, 0, rqst->data, 0, 0);
    }

    // cnfigurar credenciales antes de conectar
    strcpy(svL_paramCon.usuario_DB, "bakaa");
    strcpy(svL_paramCon.password_DB, "bakaa");
    strcpy(svL_paramCon.name_DB, "//localhost:1521/ORCLPDB1");

    conectando_aDB(&svL_paramCon);
    
    // manejador de errores de SQL
    EXEC SQL WHENEVER SQLERROR DO sqlError("Error al realizar SELECT en tiburones");

    // 3SELECT INTO mapeando las columnas a las variables de host
    EXEC SQL AT ORACLE2 
        SELECT genero, etapa, pies, pulgadas, descripcion, TO_CHAR(fecha_tag, 'YYYY-MM-DD HH24:MI:SS'), lugar_tag, id_especie
        INTO :h_genero, :h_etapa, :h_pies, :h_pulgadas, :h_descripcion, :h_fecha_tag, :h_lugar_tag, :h_id_especie
        FROM tiburones 
        WHERE nombre = :h_nombre;

    // Nota: Si no encuentra filas, sqlca.sqlcode sera 1403. Podrias manejarlo si lo deseas.
    if(sqlca.sqlcode == 1403){
        print("No se encontraron resultados para el nombre %s", NOMBRE);
    }

    cierraConexion();

    // limpiar el buffer original e inyectar las respuestas para el cliente
    Finit32(transb, Fsizeof32(transb)); // limpia el buffer para asegurar que va limpio
    
    Fadd32(transb, NOMBRE, h_nombre, 0);
    Fadd32(transb, GENERO, h_genero, 0);
    Fadd32(transb, ETAPA, h_etapa, 0);
    Fadd32(transb, PIES, (char *)&h_pies, 0);
    Fadd32(transb, PULGADAS, (char *)&h_pulgadas, 0);
    Fadd32(transb, DESCRIPCION, h_descripcion, 0);
    Fadd32(transb, FECHA_TAG, h_fecha_tag, 0);
    Fadd32(transb, LUGAR_TAG, h_lugar_tag, 0);
    Fadd32(transb, ID_ESPECIE, (char *)&h_id_especie, 0);

    // devolver exito enviando el buffer lleno de regreso al cliente
    tpreturn(TPSUCCESS, 0, (char *)transb, 0, 0);
}