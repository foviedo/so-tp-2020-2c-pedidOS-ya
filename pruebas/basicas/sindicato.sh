source pedidosya.config

echo "---------- [  CONFIGURACIÓN DE SINDICATO  ] ----------"
./sindicato/config.sh serv $PUERTO_SINDICATO
./sindicato/config.sh blk 64 1024
echo "---------- [   CONFIGURACIÓN DE CLIENTE   ] ----------"
./cliente/config.sh $IP_SINDICATO $PUERTO_SINDICATO