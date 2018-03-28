#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h> //fcntl
#include <sys/time.h>

static void usage (char *program)
{
    printf ("Usage %s port destino/gateway/máscara/interface", program);
    exit (1);
}

typedef struct{
    unsigned char priority:4, version:4;
    unsigned char flow[3];
    unsigned short int payloadLength;
    unsigned char nexthdr;
    unsigned char hoplimit;

    //unsigned int saddr[4];
    //unsigned int daddr[4];
    //uint16_t saddr[8];
    //uint16_t daddr[8];
    char saddr[16];
    char daddr[16];
}ipv6header;


int main(int argc, char** argv) {

	if(argc < 4)
        usage(argv[0]);

    //char *port = argv[1];

    //ipv6header *ip = malloc(sizeof(ipv6header));;
    ipv6header *ip = malloc(sizeof(ipv6header));

    int port = atoi(argv[1]);

    char ipDestinoFinal[16];
    char ipOrigem[16];
    int size_recv, total_size= 0; 

    /* Criando um socket primeiro */
    int sockfd = 0;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Erro : Nao foi possivel criar o socket \n");
        return 1;
    }

    /* Inicializando a estrutura de dados sockaddr_in */
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); // port
    //para usar um ip qualquer use inet_addr("10.10.10.10"); ao invés de htonl(INADDR_ANY)
    //serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

     /* Fazendo a conexao com o sender*/
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Erro : Conexao com o sender falhou \n");
        return 1;
    }

    int bytesReceived = 0;

    //faz o socket nao blockear
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
     
    while(1)
    {
        //printf("TESTE\n");
        int tam_sender = sizeof(struct sockaddr_in);
        //printf("TESTE\n");
        //size_recv = recvfrom(sockfd, ip, sizeof(ip), 0, (struct sockaddr*) &serv_addr, &tam_sender);
        
        while((size_recv = recvfrom(sockfd, ip, 40, 0, (struct sockaddr*) &serv_addr, &tam_sender) ) > 0)
        {
            total_size += size_recv;
            printf("Recebendo dados...\n");
            //printf("TESTE While\n");
            size_recv = 0; 
            //printf("\n\nIp Origem is %s", ip->saddr);
        }

        if(total_size = 0)
        {
            printf("\n Erro na recepção dos arquivos \n");
            exit(1);
        }

        if(size_recv == 0){
            break;
        }
    }

    printf("Recebi o pacote\n");

    strcpy(ipOrigem,ip->saddr);
    strcpy(ipDestinoFinal,ip->daddr);
    printf("\n\nIp Origem is %s\n", ipOrigem);
    printf("Ip Destino is %s\n\n", ipDestinoFinal);


    bool defaultRoute = false;
    bool notDefaultRoute = false;
    int indexDefaultRoute = -1;
    int numberMatch = 0;
    int longestMatch = 0;
    int indexEntradaLongestMatch = -1;
    int ponteiroFimIP = -1;
    char *gateway = malloc(15 * sizeof(1));
    char *mascara = malloc(15 * sizeof(1));
    char *interface = malloc(15 * sizeof(1));
    bool stopLongestMatch = false;
    char ipDestino[strlen(ipDestinoFinal)];

    int numeroDeEntradas = argc - 2;

    //printf("ANtes da 2 parte\n");

    for(int entradaAtual = 0; entradaAtual < numeroDeEntradas; entradaAtual++){

    	char *entrada = argv[2 + entradaAtual];

        //char possivelIpDestino[INET6_ADDRSTRLEN];
        //inet_ntop (AF_INET6, src, possivelIpDestino, sizeof (possivelIpDestino));
        //printf ("source address is %s\n", possivelIpDestino);

    	char possivelIpDestino[strlen(ipOrigem)];

        //printf("TEste 1\n");

    	int ponteiro = 0;
    	while(entrada[ponteiro] != '/'){
    		if(entrada[ponteiro] == ipDestinoFinal[ponteiro] && stopLongestMatch == false){
    			if(entrada[ponteiro] != ':'){
    				numberMatch++;
                    notDefaultRoute = true;
                    //printf("numberMatch++\n");
                }
    			possivelIpDestino[ponteiro] = entrada[ponteiro];
    			//printf(" %c %c\n", entrada[ponteiro], ipDestinoFinal[ponteiro]);
    			ponteiro++;
    			//printf("entrei\n");
    		}
    		else if((entrada[ponteiro] == ':') && notDefaultRoute == false){
    			defaultRoute = true;
    			possivelIpDestino[ponteiro] = entrada[ponteiro];
    			ponteiro++;
    			stopLongestMatch == true;
    		}
    		else{
    			notDefaultRoute = true;
    			possivelIpDestino[ponteiro] = entrada[ponteiro];
    			ponteiro++;
    			stopLongestMatch == true;
    		}
            //printf("TEste 2\n");
    	}

    	if(numberMatch > longestMatch){
    		longestMatch = numberMatch;
    		indexEntradaLongestMatch = entradaAtual + 2;
    		//free(ipDestino);
    		strcpy(ipDestino, possivelIpDestino);
    		ponteiroFimIP = ponteiro;
    	}

    	if(defaultRoute == true && notDefaultRoute == false){
    		indexDefaultRoute = entradaAtual + 2;
    		ponteiroFimIP = ponteiro;
    	}

    	//printf("Iteração: %d\n", entradaAtual + 2);
    	defaultRoute = false;
   		notDefaultRoute = false;
    	numberMatch = 0;
    	stopLongestMatch = false;
        //printf("TROCA de entrada\n\n");
        //printf("TEste 3\n");
    }

    //printf("entradaDoLongestMatch: %d\n", indexEntradaLongestMatch);
    //printf("longestMatch:%s\n", argv[indexEntradaLongestMatch]);
    //printf("Numero de entradas: %d\n", numeroDeEntradas);
    //printf("IP destino: %s\n", ipDestino);


    if(longestMatch > 0){

    	char *entrada = argv[indexEntradaLongestMatch];

		ponteiroFimIP++;

		int i = 0;
		while(entrada[ponteiroFimIP] != '/'){
			gateway[i] = entrada[ponteiroFimIP];
			ponteiroFimIP++;
			i++;
		}
		ponteiroFimIP++;
		i = 0;

		while(entrada[ponteiroFimIP] != '/'){
			mascara[i] = entrada[ponteiroFimIP];
			ponteiroFimIP++;
			i++;
		}
		ponteiroFimIP++;
		i = 0;

		while(entrada[ponteiroFimIP] != '\0'){
			interface[i] = entrada[ponteiroFimIP];
			ponteiroFimIP++;
			i++;
		}

		//printf("gateway destino: %s\n", gateway);

		//printf("longestMatch: %d\n", longestMatch);

    	printf("\npacket from %s to %s, forwarding to %s over %s\n", ipOrigem, ipDestinoFinal, gateway, interface);
    }

    else if(indexDefaultRoute != -1){
    	char *entrada = argv[indexDefaultRoute];

		ponteiroFimIP++;

		int i = 0;
		while(entrada[ponteiroFimIP] != '/'){
			gateway[i] = entrada[ponteiroFimIP];
			ponteiroFimIP++;
			i++;
		}
		ponteiroFimIP++;
		i = 0;

		while(entrada[ponteiroFimIP] != '/'){
			mascara[i] = entrada[ponteiroFimIP];
			ponteiroFimIP++;
			i++;
		}
		ponteiroFimIP++;
		i = 0;

		while(entrada[ponteiroFimIP] != '\0'){
			interface[i] = entrada[ponteiroFimIP];
			ponteiroFimIP++;
			i++;
		}

		//printf("gateway destino: %s\n", gateway);

    	printf("\npacket from %s to %s, forwarding to %s over %s\n", ipOrigem, ipDestinoFinal, gateway, interface);
    }

    else{
    	printf("\ndestination %s not found in routing table, dropping packet\n", ipDestinoFinal);
    	//printf("gateway destino: %s\n", gateway);
    	//printf("indexDefaultRoute %d\n", indexDefaultRoute);
    }

    close(sockfd);


    return 0;
}

//deu errado
//$ ./ipgen6 12345 3::4 1:3:5:7:9:b:d:f