#ifndef CONN_CORE_H_
#define CONN_CORE_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "utils/cserror.h"
#include "utils/csconfig.h"
#include "csmsg.h"

typedef volatile int t_sfd;

typedef struct
{
	uint32_t size;
	void*	 stream;
}t_buffer;

typedef struct
{
	t_header  header;
	t_buffer* payload;
}t_package;

/**
* @NAME cs_enum_opcode_to_str
* @DESC Devuelve el string correspondiente al enum value de t_enum_msgtype
*/
const char* cs_enum_opcode_to_str(int value);

/**
* @NAME cs_package_destroy
* @DESC Destruye un paquete y su contenido
*/
void cs_package_destroy(t_package* package);

/**
* @NAME cs_buffer_destroy
* @DESC Destruye un buffer y su contenido
*/
void cs_buffer_destroy(t_buffer* buffer);

/**
* @NAME cs_tcp_server_accept_routine
* @DESC Acepta conexiones al socket file descriptor que recibe por parámetro, y, con
* esa conexión aceptada, realiza la función que recibe por parámetro. En caso de error,
* llama a la función que lo maneja, pasándole por parámetro el tipo de error.
*
* NOTA: Para utilizar funciones que reciban más parámetros como "success_action()", usa
* orden superior.
*/
void cs_tcp_server_accept_routine(t_sfd* conn, void(*success_action)(t_sfd*), void(*err_handler)(e_status));

/**
* @NAME cs_tcp_server_create
* @DESC Crea un servidor (recibiendo por parámetro la dirección ip y el
* puerto) y devuelve el socket file descriptor correspondiente. Si no logra crearlo,
* retorna el tipo de error.
*/
e_status cs_tcp_server_create(t_sfd* conn, char* port);

/**
* @NAME cs_tcp_client_create
* @DESC Se conecta a un servidor (recibiendo por parámetro la dirección ip y el
* puerto) y devuelve el socket file descriptor correspondiente. Si no logra conectarse,
* retorna el tipo de error.
*/
e_status cs_tcp_client_create(t_sfd* conn, char* ip, char* port);

/**
* @NAME cs_get_peer_info
* @DESC Retorna una IP en formato string desde una dirección a partir de un
 * socket file descriptor
*/
e_status cs_get_peer_info(t_sfd sfd, char** ip_str_ptr, char** port_str_ptr);

/**
* @NAME cs_socket_is_connected
* @DESC Retorna true si el socket está conectado.
*/
bool cs_socket_is_connected(t_sfd sfd);

#endif /* CONNCORE_H_ */
