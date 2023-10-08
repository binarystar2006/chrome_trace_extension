#pragma once

#include <map>
#include "cJSON.h" // 使用cJSON库

// Chrome Trace事件数据结构
typedef enum {
A=0,
B,
C
}T_NAME_E;



struct ChromeTraceEvent {
    uint32_t name;
    char ph;
    int64_t ts;
    int pid;
    int tid;
};

class ChromeTrace {
public:
    ChromeTrace() {
        trcNameMap[A] = "alpha";
        trcNameMap[B] = "beta";
        trcNameMap[C] = "gamma";

        std::thread receiverThread(ReceiverThread);
        std::thread writerThread(WriterThread);

        receiverThread.join();
        writerThread.join();
    };

private:
    cJSON* EventToJson(const ChromeTraceEvent& event, cJSON *root);
    void ReceiverThread();
    void WriterThread();

    std::map<T_NAME_E, char[128]> trcNameMap;
    std::list<ChromeTraceEvent> eventList; // 使用std::list存储事件
    std::mutex eventMutex; // 用于互斥访问事件列表的互斥锁
}