#include "clparser.h"
#include "clmessages.h"

#define CONFIG_FILE_PATH "cliente.config"
#define LOG_FILE_KEY     "ARCHIVO_LOG"
#define IP_SERVER        "IP"
#define PORT_SERVER      "PUERTO"

pthread_t thread_recv_msg;

void client_init(void)
{
	char* module_name;

	//Inicia los config y logger
	CHECK_STATUS(cs_config_init(CONFIG_FILE_PATH));
	module_name = strdup(cs_config_get_string("ID_CLIENTE"));
	string_to_upper(module_name);
	CHECK_STATUS(cs_logger_init(LOG_FILE_KEY, module_name));

	//serv_ip y serv_port almacenan la info del servidor a conectarse
	serv_ip   = cs_config_get_string(IP_SERVER);
	serv_port = cs_config_get_string(PORT_SERVER);

	if(serv_ip == NULL || serv_port == NULL)
	{
		PRINT_ERROR(STATUS_CONFIG_ERROR);
		exit(STATUS_CONFIG_ERROR);
	}

	//Crea el primer socket para recibir mensajes ahí
	CHECK_STATUS(cs_tcp_client_create(&serv_conn, serv_ip, serv_port));
	CHECK_STATUS(client_send_handshake(serv_conn, &serv_module));
	pthread_create(&thread_recv_msg, NULL, (void*) client_recv_msg_routine, NULL);

	CS_LOG_TRACE(__FILE__":%s:%d -- Iniciado correctamente.", __func__, __LINE__);

	free(module_name);
}

int main(int argc, char* argv[])
{
	for(int i = 1; i<argc; i++)
	{
		if(string_starts_with(argv[i],"-")) cs_parse_argument(argv[i]);
	}

	client_init();

	while(true)
	{
		int    arg_cant;
		char** arg_values;

		arg_values = cs_console_readline(prompt, &arg_cant);
		if(arg_values == NULL)
		{
			continue;
		}
		else if(!strcmp(arg_values[0], "exit"))
		{
			CS_LOG_TRACE(__FILE__":%s:%d -- Se recibió el comando exit.", __func__, __LINE__);
			string_iterate_lines(arg_values, (void*) free);
			free(arg_values);
			break;
		}

		//Parsea los argumentos
		cl_parser_result *result = client_parse_arguments(arg_cant, arg_values, serv_module);
		if(result != NULL)
		{
			char* msg_to_str = cs_msg_to_str(result->msg, OPCODE_CONSULTA, result->msgtype);
			CS_LOG_TRACE(__FILE__":%s:%d -- Se parseó el mensaje: %s",  __func__, __LINE__, msg_to_str);
			free(msg_to_str);

			//Envía el mensaje
			pthread_t thread_msg;
			pthread_create(&thread_msg, NULL, (void*) client_send_msg, (void*) result);
			pthread_detach(thread_msg);
		}

		string_iterate_lines(arg_values, (void*) free);
		free(arg_values);
	}

	CS_LOG_TRACE(__FILE__":%s:%d -- Finalizando...", __func__, __LINE__);

	pthread_cancel(thread_recv_msg);
	pthread_join(thread_recv_msg, NULL);
	close(serv_conn);
	free(prompt);

	cs_module_close();

	return EXIT_SUCCESS;
}
