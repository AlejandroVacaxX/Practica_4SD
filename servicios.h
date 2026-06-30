#ifndef SERVER_H
#define SERVER_H

#include <atmi.h>


   
// Definimos la estructura común de conexión para que todos los archivos la usen
typedef struct {
    char usuario_DB[30];
    char password_DB[30];
    char name_DB[50];
} paramConexion;


// Funciones de gestión de Base de Datos que heredan los submódulos
void conectando_aDB(paramConexion *parametros);
void sqlError(char *msg);
void cierraConexion();



// Estas son las funciones reales que ejecutan el CRUD interno
void ejecutar_insert(TPSVCINFO *rqst);
void ejecutar_select(TPSVCINFO *rqst);
void ejecutar_update(TPSVCINFO *rqst);
void ejecutar_delete(TPSVCINFO *rqst);

#endif /* SERVER_H */