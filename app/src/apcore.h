#ifndef APCORE_H_
#define APCORE_H_

#include <cshared/cshared.h>

typedef struct
{
	char* nombre;
	t_pos pos;
	int restaurante_seleccionado;
	t_sfd conexion;
}ap_cliente_t;

typedef struct
{
	char* nombre;
	t_pos posicion;
	char* ip_escucha;
	char* puerto_escucha;
}ap_restaurante_t;

void ap_core_init(void);

ap_cliente_t* ap_cliente_create(char* nombre, t_pos posicion, t_sfd conexion);
void 		  ap_cliente_add(ap_cliente_t* cliente);
int  		  ap_cliente_find_index(char* cliente);

#endif /* APCORE_H_ */
