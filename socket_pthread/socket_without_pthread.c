#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "string.h"
#include "sys/time.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "sys/stat.h"
#include "unistd.h"
#include "netinet/in.h"
#include "arpa/inet.h"

#define SERVER_IP_ADDR "192.168.56.102"
#define BUFF_SIZE 1024
#define SOCKET_NUM 5
   
void* mysub(){
    int clientSocket[SOCKET_NUM]; //client socket 만들어서 fd 반환된 값,
    struct sockaddr_in serverAddr[SOCKET_NUM];
    char buff[SOCKET_NUM][BUFF_SIZE];

    //소켓 생성. AF_INET 은 IPv4 Internet protocols
    for (int i = 1; i<SOCKET_NUM+1; i++) {
        clientSocket[i] = socket(AF_INET, SOCK_STREAM, 0);
        
        //에러 체크
        if (clientSocket[i] != -1) {
            printf("Client : Open stream socket\n");
        } else {
            printf("Client : Cannot open stream socket\n");
            exit(0);
        }
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr[i].sin_family = AF_INET;
        serverAddr[i].sin_port = htons(i*1111); //포트 번호
        serverAddr[i].sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
        
        //connect and error check
        if(connect(clientSocket[i], (struct sockaddr *) &serverAddr[i], sizeof(serverAddr[i])) != -1) {
            printf("Client : Connect to server\n");
        } else {
            printf("Client : Cannot connect to server\n");
            exit(0);
        }
    }

    //write contents and arrival times of packets to the file for 3 seconds
    for (int i =1; i<SOCKET_NUM+1; i++) {
        for(int j =0; j<50; j++) {
            read(clientSocket[i], buff, BUFF_SIZE);
            printf("[PORT %d] : %s\n", i*1111, buff);
        }
    }
    return 0;
}



int main() {
    mysub();
    return 0;
}
