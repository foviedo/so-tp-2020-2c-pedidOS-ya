source pedidosya.config

echo "---------- [  CONFIGURACIÓN DE SINDICATO  ] ----------"
./config.sh sin serv $PUERTO_SINDICATO

echo "---------- [ CONFIGURACIÓN DE RESTAURANTE ] ----------"
./restaurante/config.sh sin $IP_SINDICATO $PUERTO_SINDICATO
./restaurante/config.sh serv $PUERTO_RESTAURANTE
./restaurante/config.sh ElParrillon
./restaurante/config.sh fifo
echo "---------- [   CONFIGURACIÓN DE CLIENTE   ] ----------"
./cliente/config.sh $IP_RESTAURANTE $PUERTO_RESTAURANTE