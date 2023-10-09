#pragma once

#include <map>
#include <list>
#include <mutex>
#include <thread>

#define TRACE_FILE "event.json" //定义trace文件名
#define TRACE_PORT 12345

class cJSON;
// Chrome Trace事件数据结构
typedef enum {
A=0,
B,
C
}T_NAME_E;

const char traceName[][128] {
    {"alpha"},{"beta"},{"gamma"}
};

struct ChromeTraceEvent {
    uint32_t name;
    char ph;
    int64_t ts;
    int pid;
    int tid;
};

class ChromeTrace {
public:
    ChromeTrace();
    ~ChromeTrace();

private:
    cJSON* EventToJson(const ChromeTraceEvent& event, cJSON *root);
    void ReceiverThread();
    void WriterThread();

    std::map<uint32_t, const char*> trcNameMap;
    std::list<ChromeTraceEvent> eventList; // 使用std::list存储事件
    std::mutex eventMutex; // 用于互斥访问事件列表的互斥锁

    std::thread receiverThread;
    std::thread writerThread;
};