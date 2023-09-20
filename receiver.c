#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h" // 使用cJSON库

struct ChromeTraceEvent {
    char name[128];
    char ph;
    int64_t ts;
    int pid;
    int tid;
};

int main() {
    int sockfd, newSock;
    struct sockaddr_in serverAddr, newAddr;
    socklen_t addr_size;
    struct ChromeTraceEvent event;

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
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定Socket
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind error");
        exit(1);
    }

    // 监听连接
    if (listen(sockfd, 10) == 0) {
        printf("Listening...\n");
    } else {
        printf("Error listening\n");
        exit(1);
    }

    // 接受连接
    addr_size = sizeof(newAddr);
    newSock = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);

    // 接收事件
    recv(newSock, &event, sizeof(event), 0);

    // 创建JSON对象
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", event.name);
    cJSON_AddStringToObject(root, "ph", (const char*)&event.ph);
    cJSON_AddNumberToObject(root, "ts", (double)event.ts);
    cJSON_AddNumberToObject(root, "pid", event.pid);
    cJSON_AddNumberToObject(root, "tid", event.tid);

    // 将JSON对象序列化为字符串
    char *jsonStr = cJSON_Print(root);

    // 将JSON字符串写入文件
    FILE *file = fopen("event.json", "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    fprintf(file, "%s\n", jsonStr);
    fclose(file);

    // 释放JSON对象和字符串
    cJSON_Delete(root);
    free(jsonStr);

    // 关闭Socket连接
    close(newSock);
    close(sockfd);

    return 0;
}
