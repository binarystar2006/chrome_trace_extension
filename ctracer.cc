#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include "ctrace.h"
#include "cJSON.h" // 使用cJSON库

ChromeTrace::ChromeTrace() {
        trcNameMap[A] = traceName[A];
        trcNameMap[B] = traceName[B];
        trcNameMap[C] = traceName[C];

            // 设置服务器信息
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(TRACE_PORT); // 服务器端口
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器IP

        writerThread = std::thread{&ChromeTrace::WriterThread, this};
        receiverThread = std::thread{&ChromeTrace::ReceiverThread, this};
        // receiverThread.join();
        // writerThread.join();
            // 创建Socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            perror("Error creating socket");
            exit(1);
        }

        // 连接到服务器
        while (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("Error connecting to server, retrying...");
            sleep(1);
        }
        std::cout<< "ChromeTrace Initiallized Done"<<std::endl;
}

// 函数用于将事件转换为JSON字符串
cJSON* ChromeTrace::EventToJson(const ChromeTraceEvent& event, cJSON *root) {
 // 创建JSON对象
    cJSON_AddStringToObject(root, "name", trcNameMap[event.name]);
    cJSON_AddStringToObject(root, "ph", event.ph);
    cJSON_AddNumberToObject(root, "ts", event.ts);
    if (event.ph[0]=='X')
        cJSON_AddNumberToObject(root, "dur", event.dur);
    cJSON_AddStringToObject(root, "pid", processName[event.pid]);
    cJSON_AddStringToObject(root, "tid", threadName[event.tid]);
    if (event.cat)
        cJSON_AddStringToObject(root, "cat", catName[event.cat]);
    return root;
}

// 线程函数，用于接收Socket信息并保存到std::list
void ChromeTrace::ReceiverThread() {
    long idx=0;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TRACE_PORT);
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

    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int newSock = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (newSock == -1) {
        std::cerr << "Error accepting connection" << std::endl;
        close(sockfd);
        return;
    }

    while (true) {
        ChromeTraceEvent* event = new ChromeTraceEvent();
        ssize_t bytesRead = recv(newSock, event, sizeof(*event), 0);
        if (bytesRead == sizeof(*event)) {
            std::lock_guard<std::mutex> lock(eventMutex);
            std::cout << "Receiver get "<<idx++<<"s events"<<std::endl;
            eventList.push_back(*event);
        }
    }

    close(sockfd);
}

// 线程函数，用于从std::list中读取数据包并写入文件
void ChromeTrace::WriterThread() {
    long idx=0;
    std::ofstream outputFile(TRACE_FILE, std::ios::trunc);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    outputFile <<"["<<std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待一段时间

        std::list<ChromeTraceEvent> eventsToWrite;

        // std::cout << "Writer thread wait write lock"<<std::endl;
        std::lock_guard<std::mutex> lock(eventMutex);
        // std::cout << "Writer thread get write lock"<<std::endl;
        eventsToWrite = eventList;
        eventList.clear();

        if (!eventsToWrite.empty()) {
            for (const auto& event : eventsToWrite) {
                std::cout << "Writer get "<< idx++<<"s events"<<std::endl;
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

void ChromeTrace::FillCommonEvent(ChromeTraceEvent &event, uint32_t pid, uint32_t tid, uint32_t name, uint64_t ts) {
    event.name = name;
    event.pid = pid;
    event.tid = tid;
    event.ts = ts;
}

void ChromeTrace::DurationTraceBegin(uint32_t pid, uint32_t tid, uint32_t name, uint64_t ts, uint32_t cat=0) {
    ChromeTraceEvent event;
    FillCommonEvent(event, pid, tid, name, ts);
    event.ph[0] = 'B';
    event.ph[1] = '\0';
    event.cat = cat;
    send(sockfd, &event, sizeof(event), 0);
}

void ChromeTrace::DurationTraceEnd(uint32_t pid, uint32_t tid, uint32_t name, uint64_t ts, uint32_t cat=0) {
    ChromeTraceEvent event;
    FillCommonEvent(event, pid, tid, name, ts);
    event.ph[0] = 'E';
    event.ph[1] = '\0';
    event.cat = cat;
    send(sockfd, &event, sizeof(event), 0);
}

void ChromeTrace::CompleteTrace(uint32_t pid, uint32_t tid, uint32_t name, uint64_t ts,uint64_t dur, uint32_t cat=0) {
    ChromeTraceEvent event;
    FillCommonEvent(event, pid, tid, name, ts);
    event.ph[0] = 'X';
    event.ph[1] = '\0';
    event.dur = dur;
    event.cat = cat;
    send(sockfd, &event, sizeof(event), 0);
}