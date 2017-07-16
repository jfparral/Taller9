#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*Definición de constantes*/
#define BUFFSIZE 1
#define ERROR		-1

#ifndef HOST_NAME_MAX 
#define HOST_NAME_MAX 256 
#endif

/*Prototipos de función*/
void recibirArchivo(int SocketFD, FILE *file);
void enviarConfirmacion(int SocketFD);
void enviarMD5SUM(int SocketFD);


int main(int argc, char *argv[]){
	struct sockaddr_in stSockAddr;
    struct sockaddr_in clSockAddr;
	FILE *archivo;
	char *host;
	int SocketServerFD;
	int SocketClientFD;
	int clientLen;
	int puerto;
	

	/*Verifica que el número de parametros sea el correcto*/
	if(argc == 1){
		printf("Uso: ./servidor <ip> <puerto>\n");
		exit(-1);
	}

	if(argc != 3){
		printf( "por favor especificar un numero de puerto\n");
	}

	/*Se crea el socket*/
	if((SocketServerFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == ERROR){
		perror("No se puede crear el socket");
		exit(EXIT_FAILURE);
	}//End if-SocketFD
 
	/*Se configura la dirección del socket*/
	memset(&stSockAddr, 0, sizeof stSockAddr);
	puerto = atoi(argv[2]);
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(puerto);
	stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
	if(bind(SocketServerFD,(struct sockaddr *)&stSockAddr, sizeof stSockAddr) == ERROR){
		perror("Error en bind");
		close(SocketServerFD);
		exit(EXIT_FAILURE);
	}//End if-bind
	inet_pton(AF_INET, argv[1], &stSockAddr.sin_addr);
	printf("Socket atado a la dirección %s\n",(char *)inet_ntoa(stSockAddr.sin_addr));
	
	//darle el nombre del puerto que esta escuchando
	if ((host = malloc(HOST_NAME_MAX)) == NULL) 
		printf(" malloc error"); 
	gethostname( host, HOST_NAME_MAX);	
	printf("Nombre del host: %s\n",host);

	if(listen(SocketServerFD, 10) == ERROR){
		perror("Error listen");
		close(SocketServerFD);
		exit(EXIT_FAILURE);
	}//End if-listen

	while (1){
		clientLen = sizeof(clSockAddr);

		//Espera por la conexión de un cliente//
		SocketClientFD = accept(SocketServerFD, 
						    (struct sockaddr *) &clSockAddr,(socklen_t *)
						    &clientLen);
		if (SocketClientFD < 0){
			perror("Fallo para aceptar la conexión del cliente");
		}//End if-accept

		/*Se configura la dirección del cliente*/
		clSockAddr.sin_family = AF_INET;
		clSockAddr.sin_port = htons(puerto);
		printf("Cliente conectado: %s\n",inet_ntoa(clSockAddr.sin_addr));

		/*Se recibe el archivo del cliente*/
		recibirArchivo(SocketClientFD,archivo);
		

	}//End infinity while

 	close(SocketClientFD);
	close(SocketServerFD);
	return 0;
}//End main program

void recibirArchivo(int SocketFD, FILE *file){
	char buffer[BUFFSIZE];
	int recibido = -1;

	/*Se abre el archivo para escritura*/
	file = fopen("archivoRecibido","wb");
	enviarConfirmacion(SocketFD);
	enviarMD5SUM(SocketFD);
	while((recibido = recv(SocketFD, buffer, BUFFSIZE, 0)) > 0){
		//printf("%s",buffer);
		fwrite(buffer,sizeof(char),1,file);
	}//Termina la recepción del archivo

	fclose(file);
	

}//End recibirArchivo procedure

void enviarConfirmacion(int SocketFD){
	char mensaje[80] = "Paquete Recibido";
	printf("\nConfirmación enviada\n");
	if(write(SocketFD,mensaje,sizeof(mensaje)) == ERROR)
			perror("Error al enviar la confirmación:");

	
}//End enviarConfirmacion

void enviarMD5SUM(int SocketFD){
	FILE *tmp;//Apuntador al archivo temporal que guarda el MD5SUM del archivo.
	char *fileName = "verificacion";
	char md5sum[80];
	system("md5sum archivoRecibido >> verificacion");
	
	tmp = fopen(fileName,"r");
	fscanf(tmp,"%s",md5sum);	
	printf("\nMD5SUM:%s\n",md5sum);	
	write(SocketFD,md5sum,sizeof(md5sum));
	fclose(tmp);

}//End enviarMD5DUM
