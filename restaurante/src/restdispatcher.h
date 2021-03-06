#ifndef RESTDISPATCHER_H_
#define RESTDISPATCHER_H_

#include "restcore.h"
#include "restenvio.h"

typedef struct
{
	int id;
	rest_estado_e estado;
	t_list* pasos_restantes;
	char* comida;
	uint32_t pedido_id;
	char* cliente;
}rest_pcb_t;

typedef struct
{
	sem_t inicio_ejecucion;
	sem_t fin_ejecucion;
	sem_t inicio_derivacion;
	sem_t fin_derivacion;
	sem_t inicio_extraccion;
	sem_t fin_extraccion;
}rest_ciclo_t;

typedef struct
{
	t_list*         comidas;
	t_queue*        queue;
	pthread_mutex_t mutex_queue;
}rest_cola_ready_t;

typedef struct
{
	rest_ciclo_t*      sem_ciclo;
	rest_pcb_t*        asignado;
	rest_cola_ready_t* ready;
}rest_dispatcher_t;

uint32_t rest_dispatcher_init(t_rta_obt_rest* metadata);

void rest_iniciar_ciclo_cpu(void);
void rest_esperar_fin_ciclo_cpu(void);

int rest_derivar_pcb(rest_pcb_t* pcb);

#endif /* RESTDISPATCHER_H_ */
