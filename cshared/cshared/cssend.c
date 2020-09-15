#include "cssend.h"
#include "utils/cslog.h"

#define CS_STRING_SIZE(str_length)\
	(sizeof(uint32_t) + str_length)

#define CS_HEADER_FIXED_SIZE\
	(sizeof(int8_t) + sizeof(int8_t))

#define CS_PAYLOAD_SIZE\
	(sizeof(uint32_t) + package->payload->size)

static e_status   cs_send_all(t_sfd conn, t_buffer* buffer);
static t_package* cs_package_create(t_header header, t_buffer* payload);
static t_buffer*  cs_package_to_buffer(t_package* package);

e_status cs_send_msg(t_sfd conn, t_header header, void* msg)
{
	e_status status = STATUS_SUCCESS;

	t_buffer  *payload = NULL;
	t_package *package = NULL;
	t_buffer  *buffer  = NULL;

	//Se pasa el mensaje a payload
	payload = cs_msg_to_buffer(header, msg);

	//Se arma el paquete y se serializa
	package = cs_package_create(header, payload);
	buffer  = cs_package_to_buffer(package);

	//Se envía al receptor
	status = cs_send_all(conn, buffer);

	//Se liberan los recursos
	cs_buffer_destroy(buffer);
	cs_package_destroy(package);
	cs_buffer_destroy(payload);

	return status;
}

static e_status cs_send_all(t_sfd conn, t_buffer* buffer)
{
	e_status status = STATUS_SUCCESS;

	uint32_t bytes_sent = 0;
	uint32_t bytes_left;
	uint32_t n;

	bytes_left = buffer->size;
	do
	{	//'MSG_NOSIGNAL' para que no se envíe la señal 'SIGPIPE' al destinatario
		n = send(conn, (buffer->stream) + bytes_sent, bytes_left, MSG_NOSIGNAL);
		if(n == -1)
		{
			cs_set_local_err(errno);
			if(errno == EPIPE)
				status = STATUS_CONN_LOST;
			else
				status = STATUS_SEND_ERROR;
		} else
		{
			bytes_sent += n;
			bytes_left -= n;
		}
	//El protocolo TCP puede partir el paquete si es muy grande (>1K)
	} while(bytes_sent < buffer->size && status == STATUS_SUCCESS);

	return status;
}

static t_package* cs_package_create(t_header header, t_buffer* payload)
{
	t_package* package;

	package = malloc(sizeof(t_package));

    package->header.opcode  = header.opcode;
    package->header.msgtype = header.msgtype;
    if(payload)
    {
    	package->payload = malloc(sizeof(t_buffer));
    	package->payload->stream = malloc(payload->size);
        memcpy(package->payload->stream, payload->stream, payload->size);
        package->payload->size = payload->size;
    } else
    {
    	package->payload = NULL;
    }

    return package;
}

static t_buffer* cs_package_to_buffer(t_package* package)
{
	t_buffer* buffer;
	int offset = 0;

	//Se reserva la memoria necesaria
	buffer = malloc(sizeof(t_buffer));

	buffer->size = CS_HEADER_FIXED_SIZE;
	if(package->payload) buffer->size += CS_PAYLOAD_SIZE;
	buffer->stream = malloc(buffer->size);

	cs_stream_copy(buffer->stream, &offset, &package->header.opcode,  sizeof(int8_t), 1);
	cs_stream_copy(buffer->stream, &offset, &package->header.msgtype, sizeof(int8_t), 1);

	//El mensaje a enviar puede no requerir de payload (ej: una suscripción)
	if(package->payload)
	{
		cs_stream_copy(buffer->stream, &offset, &package->payload->size,  sizeof(uint32_t), 1);
		cs_stream_copy(buffer->stream, &offset, package->payload->stream, package->payload->size, 1);
	}

	return buffer;
}

static t_buffer* cs_solicitud_to_buffer(t_solicitud* msg);
static t_buffer* cs_rta_cons_rest_to_buffer(t_rta_cons_rest* msg);
static t_buffer* cs_rta_obt_rest_to_buffer(t_rta_obt_rest* msg);
static t_buffer* cs_rta_cons_pl_to_buffer(t_rta_cons_pl* msg);
static t_buffer* cs_rta_crear_ped_to_buffer(t_rta_crear_ped* msg);
static t_buffer* cs_rta_cons_ped_to_buffer(t_rta_cons_ped* msg);
static t_buffer* cs_rta_obt_ped_to_buffer(t_rta_obt_ped* msg);
static t_buffer* cs_rta_obt_rec_to_buffer(t_rta_obt_rec* msg);

t_buffer* cs_msg_to_buffer(t_header header, void* msg)
{
	switch(header.opcode)
	{
	case OPCODE_SOLICITUD:
		return cs_solicitud_to_buffer((t_solicitud*)msg);
	case OPCODE_RESPUESTA_OK:
		switch(header.msgtype)
		{
		case CONSULTAR_RESTAURANTES:
			return cs_rta_cons_rest_to_buffer((t_rta_cons_rest*)msg);
		case OBTENER_RESTAURANTE:
			return cs_rta_obt_rest_to_buffer((t_rta_obt_rest*)msg);
		case CONSULTAR_PLATOS:
			return cs_rta_cons_pl_to_buffer((t_rta_cons_pl*)msg);
		case CREAR_PEDIDO:
			return cs_rta_crear_ped_to_buffer((t_rta_crear_ped*)msg);
		case CONSULTAR_PEDIDO:
			return cs_rta_cons_ped_to_buffer((t_rta_cons_ped*)msg);
		case OBTENER_PEDIDO:
			return cs_rta_obt_ped_to_buffer((t_rta_obt_ped*)msg);
		case OBTENER_RECETA:
			return cs_rta_obt_rec_to_buffer((t_rta_obt_rec*)msg);
		default:
			return NULL;
		}
		break;
	default:
		break;
	}
	return NULL;
}

static t_buffer* cs_solicitud_to_buffer(t_solicitud* msg)
{
	t_buffer *buffer;
	int offset = 0;

	buffer = malloc(sizeof(t_buffer));
	buffer->size = 4 * sizeof(uint32_t) + msg->comida_length + msg->restaurante_length;
	buffer->stream = malloc(buffer->size);

	cs_stream_copy(buffer->stream, &offset, &msg->comida_length     , sizeof(uint32_t)       , COPY_SEND);
	cs_stream_copy(buffer->stream, &offset,  msg->comida            , msg->comida_length     , COPY_SEND);
	cs_stream_copy(buffer->stream, &offset, &msg->cantidad          , sizeof(uint32_t)       , COPY_SEND);
	cs_stream_copy(buffer->stream, &offset, &msg->restaurante_length, sizeof(uint32_t)       , COPY_SEND);
	cs_stream_copy(buffer->stream, &offset,  msg->restaurante       , msg->restaurante_length, COPY_SEND);
	cs_stream_copy(buffer->stream, &offset, &msg->pedido_id         , sizeof(uint32_t)       , COPY_SEND);

	return buffer;
}

static t_buffer* cs_rta_cons_rest_to_buffer(t_rta_cons_rest* msg)
{
	t_buffer *buffer;
	//int offset = 0;

	buffer = malloc(sizeof(t_buffer));

	//TODO: [MSG->BUFFER] cs_rta_cons_rest_to_buffer

	return buffer;
}

static t_buffer* cs_rta_obt_rest_to_buffer(t_rta_obt_rest* msg)
{
	t_buffer *buffer;
	//int offset = 0;

	buffer = malloc(sizeof(t_buffer));

	//TODO: [MSG->BUFFER] cs_rta_obt_rest_to_buffer

	return buffer;
}

static t_buffer* cs_rta_cons_pl_to_buffer(t_rta_cons_pl* msg)
{
	t_buffer *buffer;
	//int offset = 0;

	buffer = malloc(sizeof(t_buffer));

	//TODO: [MSG->BUFFER] cs_rta_cons_pl_to_buffer

	return buffer;
}

static t_buffer* cs_rta_crear_ped_to_buffer(t_rta_crear_ped* msg)
{
	t_buffer *buffer;
	//int offset = 0;

	buffer = malloc(sizeof(t_buffer));

	//TODO: [MSG->BUFFER] cs_rta_crear_ped_to_buffer

	return buffer;
}

static t_buffer* cs_rta_cons_ped_to_buffer(t_rta_cons_ped* msg)
{
	t_buffer *buffer;
	//int offset = 0;

	buffer = malloc(sizeof(t_buffer));

	//TODO: [MSG->BUFFER] cs_rta_cons_ped_to_buffer

	return buffer;
}

static t_buffer* cs_rta_obt_ped_to_buffer(t_rta_obt_ped* msg)
{
	t_buffer *buffer;
	//int offset = 0;

	buffer = malloc(sizeof(t_buffer));

	//TODO: [MSG->BUFFER] cs_rta_obt_ped_to_buffer

	return buffer;
}

static t_buffer* cs_rta_obt_rec_to_buffer(t_rta_obt_rec* msg)
{
	t_buffer *buffer;
	//int offset = 0;

	buffer = malloc(sizeof(t_buffer));

	//TODO: [MSG->BUFFER] cs_rta_obt_rec_to_buffer

	return buffer;
}
