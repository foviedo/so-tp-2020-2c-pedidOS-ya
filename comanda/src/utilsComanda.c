#include "utilsComanda.h"

void server_send_rta_ok_fail(e_msgtype msg_type, t_sfd client_conn,e_opcode ok_fail)
{
	t_header header= {ok_fail,msg_type};
	char* rta_to_str = cs_msg_to_str(NULL, header.opcode, header.msgtype);
	if( cs_send_respuesta(client_conn, header, NULL) == STATUS_SUCCESS )
		{
			CS_LOG_INFO("Se envió la respuesta: %s", rta_to_str);
		} else
		{
			CS_LOG_ERROR("No se pudo enviar la respuesta: %s", rta_to_str);
		}

		free(rta_to_str);
		cs_msg_destroy(NULL, header.opcode, header.msgtype);

}

t_restaurante* buscarRestaurante(char* restaurante){
	int tamanioSegmentos= list_size(listaRestaurantes);
	t_restaurante* unRestaurante;
	for (int i=0; i<tamanioSegmentos; i++){
		unRestaurante = list_get(listaRestaurantes,i);
		printf("voy a comparar %s, %s",unRestaurante->nombreRestaurante,restaurante);
		if(!strcmp((unRestaurante->nombreRestaurante),restaurante)){
			return unRestaurante;
			CS_LOG_TRACE("voy a retornar un restaurante bueno");
		}
	}
	return (t_restaurante*) 0;
}

t_segmentoPedido* buscarPedido(uint32_t pedido_id, t_restaurante* unRest){
	int tamListaPedidos = list_size(unRest->pedidos);
	for (int i=0;i<tamListaPedidos;i++){
		t_segmentoPedido* unPedido = list_get(unRest->pedidos,i);
		if (unPedido->idPedido == pedido_id){
			return unPedido;
		}
	}
	return NULL;
}

e_opcode guardarPedido(t_consulta* msg){
	t_restaurante* restaurante;
	restaurante = buscarRestaurante(msg->restaurante);
	if(!restaurante){
		CS_LOG_TRACE("no encontre el restaurante");
		restaurante = malloc(sizeof(t_restaurante));
		restaurante->nombreRestaurante = string_duplicate(msg->restaurante);
		restaurante->pedidos = list_create();
		list_add(listaRestaurantes,restaurante);
		t_segmentoPedido* pedido = malloc(sizeof(t_segmentoPedido));
		pedido->idPedido = msg->pedido_id;
		pedido->tablaPaginas = list_create();
		pedido->estadoPedido = PEDIDO_PENDIENTE;
		list_add(restaurante->pedidos,pedido);
		return OPCODE_RESPUESTA_OK;
	} else{
		CS_LOG_TRACE("encontre el restaurante, voy a crear el pedido");
		t_segmentoPedido* pedido = buscarPedido(msg->pedido_id, restaurante);
		if(pedido){ //si encontro el pedido
			return OPCODE_RESPUESTA_FAIL;
		}
		pedido = malloc(sizeof(t_segmentoPedido));
		pedido->idPedido = msg->pedido_id;
		pedido->tablaPaginas = list_create();
		pedido->estadoPedido = PEDIDO_PENDIENTE;
		list_add(restaurante->pedidos,pedido);
		return OPCODE_RESPUESTA_OK;
	}
}

t_list* acomodarFrames(int tamMemoria){
	int cantidadDeFrames = tamMemoria/32;
	t_list* frames = list_create();
	int offset = 0;
	for(int i=0; i<cantidadDeFrames; i++){
		t_frame_en_memoria* unFrame = malloc(sizeof(t_frame_en_memoria));
		unFrame->inicio = memoriaPrincipal + offset;
		offset+=32;
		unFrame->paginaALaQuePertenece = NULL;
		list_add(frames,unFrame);
	}
//	printf("LA memoria incia en %i, el ultimo frame esta en en %i", (int)memoriaPrincipal, (int) (((t_frame_en_memoria*)list_get(frames,31))->inicio));
	return frames;
}

t_frame_en_memoria* dameUnFrame(){
	int tamanioListaFrames = list_size(listaFramesMemoria);
	for (int i=0;i<tamanioListaFrames;i++){
		t_frame_en_memoria* unFrame;
		unFrame = list_get(listaFramesMemoria,i);
		if(unFrame->paginaALaQuePertenece == NULL){
			return unFrame;
		}
	}
}



e_opcode guardarPlato(t_consulta* msg){
	t_restaurante* unRest = buscarRestaurante(msg->restaurante); //1
	if(!unRest){
		return OPCODE_RESPUESTA_FAIL;
	}
	t_segmentoPedido* pedido = buscarPedido(msg->pedido_id,unRest); //2
	if(!pedido){
		return OPCODE_RESPUESTA_FAIL;
	}
	//TODO: aca estoy suponiendo que el plato no existe, tengo que ver el caso que si existe //3
	int existe=0;
	if(existe){

	} else{
		t_frame_en_memoria* frameAPisar = dameUnFrame();
		t_pagina* pagina = malloc(sizeof(t_pagina));
		pagina->inicioMemoria = frameAPisar->inicio;
		pagina->numeroPagina= list_size(pedido->tablaPaginas);
		pagina->presente =1;
		frameAPisar->paginaALaQuePertenece = pagina;
		int offset=0;
		memcpy(pagina->inicioMemoria + offset,&(msg->cantidad),sizeof(int));
		offset+=sizeof(uint32_t);
		int cero = 0;
		memcpy(pagina->inicioMemoria + offset,&cero,sizeof(int));
		offset+=sizeof(uint32_t);
		memcpy(pagina->inicioMemoria+offset,msg->comida,strlen(msg->comida)+1);

	}
	//TODO: cosas de swap //4
	return OPCODE_RESPUESTA_OK;


}
