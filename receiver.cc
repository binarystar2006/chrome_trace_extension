#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <cstring>
#include <list>
#include <fstream>
#include <mutex>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include "ctrace.h"

// 函数用于将事件转换为JSON字符串
cJSON* ChromeTrace::EventToJson(const ChromeTraceEvent& event, cJSON *root) {
 // 创建JSON对象
    cJSON_AddStringToObject(root, "name", event.name);
    cJSON_AddStringToObject(root, "ph", (const char*)&event.ph);
    cJSON_AddNumberToObject(root, "ts", (double)event.ts);
    cJSON_AddNumberToObject(root, "pid", event.pid);
    cJSON_AddNumberToObject(root, "tid", event.tid);
    return root;
}

// 线程函数，用于接收Socket信息并保存到std::list
void ChromeTrace::ReceiverThread() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(sockfd);
        return;
    }

    if (listen(sockfd, 10) == 0) {
        std::cout << "Listening..." << std::endl;
    } else {
        std::cerr << "Error listening" << std::endl;
        close(sockfd);
        return;
    }

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int newSock = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (newSock == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            close(sockfd);
            return;
        }

        ChromeTraceEvent* event = new ChromeTraceEvent();
        ssize_t bytesRead = recv(newSock, event, sizeof(*event), 0);
        if (bytesRead == sizeof(*event)) {
            std::lock_guard<std::mutex> lock(eventMutex);
            eventList.push_back(*event);
        }

        close(newSock);
    }

    close(sockfd);
}

// 线程函数，用于从std::list中读取数据包并写入文件
void ChromeTrace::WriterThread() {
    std::ofstream outputFile("event.json", std::ios::trunc);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    outputFile <<"["<<std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待一段时间

        std::list<ChromeTraceEvent> eventsToWrite;
        
        {
            std::lock_guard<std::mutex> lock(eventMutex);
            eventsToWrite = eventList;
            eventList.clear();
        }

        if (!eventsToWrite.empty()) {
            for (const auto& event : eventsToWrite) {
                cJSON *root = cJSON_CreateObject();
                cJSON* jsonEvent = EventToJson(event, root);
                    // 将JSON对象序列化为字符串
                char *jsonStr = cJSON_Print(jsonEvent);
                outputFile << jsonStr <<"," <<std::endl;
                cJSON_Delete(jsonEvent);
            }
        }
    }
    outputFile.close();
}
