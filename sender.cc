#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ctrace.h"
#include <iostream>

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    ChromeTrace ctce_rcv;
    long idx=0;

    // 创建Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // 设置服务器信息
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TRACE_PORT); // 服务器端口
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器IP

    // 连接到服务器
    while (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        sleep(1);
    }

    for(int i=1; i<100;i++) {
        // 创建一个Chrome Trace事件并发送
        struct ChromeTraceEvent* event = new ChromeTraceEvent();
        event->name= B;
        event->ph = 'B';
        event->ts = 500*i+100 ;
        event->pid = getpid();
        event->tid = 0; // 如果有多个线程，可以设置不同的线程ID

        // 发送事件到服务器
        send(sockfd, event, sizeof(*event), 0);
        std::cout << "Sending " << idx++<<" events"<<std::endl;

        event->name= B;
        event->pid = getpid();
        event->tid = 0; // 如果有多个线程，可以设置不同的线程ID
        event->ph = 'E';
        event->ts = 500*i+250;

        // 发送事件到服务器
        send(sockfd, event, sizeof(*event), 0);
        std::cout << "Sending " << idx++<<" events"<<std::endl;
        event->pid = getpid();
        event->tid = 1; // 如果有多个线程，可以设置不同的线程ID
        event->name = A;
        event->ph = 'B';
        event->ts = 500*i+300;

        // 发送事件到服务器
        send(sockfd, event, sizeof(*event), 0);
        std::cout << "Sending " << idx++<<" events"<<std::endl;

        event->pid = getpid();
        event->tid = 1; // 如果有多个线程，可以设置不同的线程ID
        event->name = A;
        event->ph = 'E';
        event->ts = 500*i+480;

        // 发送事件到服务器
        send(sockfd, event, sizeof(*event), 0);
        std::cout << "Sending " << idx++<<" events"<<std::endl;
    }
    while(1){;}
    // 关闭Socket连接
    close(sockfd);

    return 0;
}
