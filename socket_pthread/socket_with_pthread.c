//
//  main.c
//  MYMY2
//
//  Created by 강수진 on 2019/11/04.
//  Copyright © 2019 강수진. All rights reserved.
//

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

void* mysub(void *portNum){
    int myPortNum = *(int *)portNum;
    printf("myport num is [%d]\n",myPortNum);
    int clientSocket; //client socket 만들어서 fd 반환된 값,
    struct sockaddr_in serverAddr;
    char buff[BUFF_SIZE];
    struct timeval tv;
    struct tm *tm;
    
    //소켓 생성. AF_INET 은 IPv4 Internet protocols
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    //에러 체크
    if (clientSocket != -1) {
        printf("Client : Open stream socket\n");
    } else {
        printf("Client : Cannot open stream socket\n");
        exit(0);
    }
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(myPortNum); //포트 번호
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
    
    //connect and error check
    if(connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != -1) {
        printf("Client [PORT %d] : Connect to server\n", myPortNum);
    } else {
        printf("Client [PORT %d] : Cannot connect to server\n", myPortNum);
        exit(0);
    }
    
    char outputFileName[10];
    sprintf(outputFileName, "%d.txt", myPortNum);
    FILE *outputFile;
    outputFile = fopen(outputFileName,"w");
    printf("file open \n");
    
    while(1){
        //읽기
        read(clientSocket, buff, BUFF_SIZE);
        // added by euncho 2019.11.04.
        
        // GET TIME
        gettimeofday(&tv, NULL);
        tm = localtime(&tv.tv_sec);
        // add end
        
        //파일에 작성
        // modified by euncho 2019.11.04.
        // print time in file
        printf("received packet. Press Ctrl+C to stop \n");
        fprintf(outputFile, "%02d:%02d:%d.%03ld %ld %s \n", tm->tm_hour, tm->tm_min, tm->tm_sec, (tv.tv_usec)/1000, strlen(buff), buff);
        
    }
    // modify end
    fclose(outputFile);
    return 0;
}


int main() {
    pthread_t p_thread[SOCKET_NUM];
    int portNumber[5];
    
    for(int i=0;i<SOCKET_NUM;i++){
        portNumber[i] = (i+1)*1111;
    }
    
    //create thread
    for(int i=0;i<SOCKET_NUM;i++){
        if ((pthread_create(&p_thread[i], NULL, mysub, &portNumber[i])) != 0) {
            printf("create thread error\n");
            return 0;
        }
    }
    
    //join thread
    for(int i=0;i<SOCKET_NUM;i++) {
        if (pthread_join(p_thread[i], NULL) != 0) {
            printf("join thread error\n");
            return 0;
        }
    }
    
    return 0;
}
