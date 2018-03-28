#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static void usage (char *program)
{
    printf ("Usage ./%s porta enderecoIPv6 enderecoIPv6", program);
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

struct icmpv6header {
    unsigned char type;
    unsigned char code;
    unsigned short int chk_sum;
    unsigned int body; 
};

int main(int argc, char** argv)
{

    if(argc < 3)
        usage(argv[0]);

    int port = atoi(argv[1]);
    //uint16_t ipsrc = htons(argv[2]);
    //uint16_t ipdst = htons(argv[3]);

    //char* packet = (char*) malloc(sizeof(struct ipv6header)+sizeof(struct icmpv6header));
    ipv6header ip;
    //struct icmpv6header* icmp = (struct icmpv6header*) (packet+sizeof(struct ipv6header));

    /*
    icmp->type = 128;
    icmp->code = 0;
    icmp->chk_sum = (0x6a13);
    icmp->body = htonl(1234);
    */

    ip.version = 6;
    ip.priority = 0;
    (ip.flow)[0] = 0;
    (ip.flow)[1] = 0;
    (ip.flow)[2] = 0;
    ip.payloadLength = ((unsigned short int) sizeof(struct icmpv6header));
    ip.nexthdr = 0;
    ip.hoplimit = 1;

    strcpy(ip.saddr, argv[2]);
    printf("IP origem %s\n", ip.saddr);
    strcpy(ip.daddr, argv[3]);
    printf("IP destino %s\n", ip.daddr);

    //char src [16];
    /*if (inet_pton (AF_INET6, argv [2], ip.saddr) != 1) {
        printf ("usage: %s udp-port ipv6-source ipv6-dest\n", argv [0]);
        printf ("       source address %s is not a valid IPv6 address", argv [2]);
        return 1;
    }*/

    //char drc [16];
    /*if (inet_pton (AF_INET6, argv [3], ip.daddr) != 1) {
        printf ("usage: %s udp-port ipv6-source ipv6-dest\n", argv [0]);
        printf ("       source address %s is not a valid IPv6 address", argv [2]);
        return 1;
    }*/

    int listenfd = socket(AF_INET, SOCK_STREAM,0);
    //sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if(listenfd == -1)
    {
        printf("Error setting listenfd socket\n");
        return -1;
    }

    struct sockaddr_in remote;
    memset(&remote, '0', sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = htonl(INADDR_ANY);
    //remote.sin6_flowinfo = 0;
    //remote.sin6_scope_id = 0;
    remote.sin_port = htons(port);

    int slen = sizeof(remote);

    bind(listenfd, (struct sockaddr*)&remote,sizeof(remote));
    if(listen(listenfd, 10) == -1)
    {
        printf("Falha ao escutar\n");
        return -1;
    }


    int connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);


    //uint16_t ipdst[8] = {htons(0x2001),htons(0x470),htons(x),htons(x),htons(y),htons(y),htons(y),htons(0xdd7b)};
    //uint16_t ipsrc[8] = {htons(0x2001),htons(0x470),htons(x),htons(x),htons(0xbee),htons(0xbee),htons(0xbee),htons(0xbee)};

    /*
    inet_pton(AF_INET6, "2001:470:x:x:y:y:y:dd7b", &(remote.sin6_addr));
    inet_pton(AF_INET6, "2001:470:x:x:bee:bee:bee:bee", &(ip->saddr));
    inet_pton(AF_INET6, "2001:470:x:x:y:y:y:dd7b", &(ip->daddr));
    */

    /*
    int j = 0;
    for(j=0;j<=7;j++) {
        remote.sin6_addr.s6_addr16[j]=ipdst[j];
        ip->saddr[j]=ipsrc[j];
        ip->daddr[j]=ipdst[j];
    }
    */

    //int sock, 
    //int optval;
    //sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    //sock = socket(AF_INET6, SOCK_RAW, IPPROTO_RAW);
    //sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    /*if(sock == -1)
    {
        printf("Error setting socket\n");
        return -1;
    }*/

    /*
    if(connect(sock, (struct sockaddr_in6 *)&remote, sizeof(remote))<0)
    {
        printf("\n Erro : Conexao falhou \n");
        return 1;
    }*/

    // Bind the socket descriptor to the source address.
    /*if (bind (sock, (struct sockaddr*)&remote, sizeof(remote)) != 0) {
        fprintf (stderr, "Failed to bind the socket descriptor to the source address.\n");
        exit (EXIT_FAILURE);
    }*/

    /*
    int ret = setsockopt(listenfd, IPPROTO_IPV6, IP_HDRINCL, &optval, sizeof(int));
    if(ret != 0) {
        printf("Error setting options %d\n", ret);
        return -1;
    }
    printf("Socket options done\n");
    */

    printf("I am about to send Packet to port %d\n",
             port);

    int ret = sendto(connfd, (struct ipv6header*)&ip, sizeof(ip), 0, (struct sockaddr *) &remote, sizeof(remote));
    printf("RET: %d\n", ret);
    if(ret != sizeof(ip)) {
        printf("Packet not sent : %d (%d)\n",ret,errno);
        return -1;
    }
    printf("Packet sent\n");

    close(listenfd);
    close(connfd);

    return 0;
}