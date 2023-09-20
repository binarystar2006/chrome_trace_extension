#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

struct ChromeTraceEvent {
    char name[128];
    char ph;
    int64_t ts;
    int pid;
    int tid;
};

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;

    // 创建Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // 设置服务器信息
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // 服务器端口
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器IP

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        exit(1);
    }

    // 创建一个Chrome Trace事件并发送
    struct ChromeTraceEvent* event = malloc(sizeof(struct ChromeTraceEvent));
    strcpy(event->name, "MyEvent");
    event->ph = 'B';
    event->ts = 123456789;
    event->pid = getpid();
    event->tid = 0; // 如果有多个线程，可以设置不同的线程ID

    // 发送事件到服务器
    send(sockfd, event, sizeof(*event), 0);

    // 关闭Socket连接
    close(sockfd);

    return 0;
}
