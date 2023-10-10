#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ctrace.h"
#include <iostream>

int main() {


    ChromeTrace ctce_rcv;
    long idx=0;

    int sockfd = ctce_rcv.GetSockfd();
    for(int i=1; i<100;i++) {
        // 创建一个Chrome Trace事件并发送
        struct ChromeTraceEvent* event = new ChromeTraceEvent();
        event->name= B;
        event->ph = 'B';
        event->ts = 500*i+100 ;
        event->pid = 0;
        event->tid = 0; // 如果有多个线程，可以设置不同的线程ID
        event->cat = 0;

        // 发送事件到服务器
        send(sockfd, event, sizeof(*event), 0);
        std::cout << "Sending " << idx++<<" events"<<std::endl;

        event->name= B;
        event->pid = 0;
        event->tid = 0; // 如果有多个线程，可以设置不同的线程ID
        event->ph = 'E';
        event->ts = 500*i+250;

        // 发送事件到服务器
        send(sockfd, event, sizeof(*event), 0);
        std::cout << "Sending " << idx++<<" events"<<std::endl;
        event->pid = 1;
        event->tid = 2; // 如果有多个线程，可以设置不同的线程ID
        event->name = A;
        event->ph = 'B';
        event->ts = 500*i+300;
        event->cat = 1;
        // 发送事件到服务器
        send(sockfd, event, sizeof(*event), 0);
        std::cout << "Sending " << idx++<<" events"<<std::endl;

        event->pid = 1;
        event->tid = 2; // 如果有多个线程，可以设置不同的线程ID
        event->name = A;
        event->ph = 'E';
        event->ts = 500*i+480;

        // 发送事件到服务器
        send(sockfd, event, sizeof(*event), 0);
        std::cout << "Sending " << idx++<<" events"<<std::endl;
    }
    while(1){;}

    return 0;
}
