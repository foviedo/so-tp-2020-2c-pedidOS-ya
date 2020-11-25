#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "sindicato.h"

// -------- LEVANTAR FILESYSTEM -------- //

void crearDirectorioAFIP();
void crearMetadata(char*);
void crearFiles(char*);
void crearBlocks(char*);
void generarBitmap(char*, t_config*);

// -------- MENSAJES -------- //

t_rta_cons_pl* consultarPlatos(t_consulta*);
e_opcode guardarPedido(t_consulta*);
e_opcode guardarPlato(t_consulta*);
e_opcode confirmarPedido(t_consulta*);
t_rta_obt_ped* obtenerPedido(t_consulta*); // Esta bien esto?
//e_opcode+t_rta_cons_ped obtener_pedido(t_consulta*);
t_rta_obt_rest* obtenerRestaurante(t_consulta*);
e_opcode platoListo(t_consulta*);
t_rta_obt_rec* obtenerReceta(t_consulta*);

//TERMINAR PEDIDO?

// -------- CONSOLA -------- //

void crearRestaurante(char**);
void crearReceta(char**);


// -------- MANEJO BITMAP -------- //

int obtenerYEscribirProximoDisponible(char*);
void eliminarBit(int);

// -------- RECETA -------- //

t_rta_obt_rec* leerReceta(char*);

// -------- PEDIDO -------- //

int existePedido(int, char*);
void escribirInfoPedido(char*, int, char*);

// -------- BLOQUES -------- //

int escribirBloques(char*, char*);
void escribirBloque(char*, int);
char* leerBloques(int);

// -------- RESTAURANTE -------- //

char* obtenerPathRestaurante(char*);
uint32_t obtenerCantidadPedidos(char*);

// -------- AUX -------- //

int existeDirectorio(char*, int);
int cantidadDeBloques(char**);
int tamanioDeBloque(char*);
void liberar_lista(char**);

#endif /* FILESYSTEM_H_ */