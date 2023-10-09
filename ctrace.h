#pragma once

#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <arpa/inet.h>
#include <netinet/in.h>

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

const char catName[][128] {
    {"first"},{"second"},{"third"}
};
struct ChromeTraceEvent {
    uint32_t name;
    char ph;
    int64_t ts;
    int pid;
    int tid;
    uint32_t cat;
};

class ChromeTrace {
public:
    ChromeTrace();
    ~ChromeTrace() {
        // 关闭Socket连接
        if(sockfd)
            close(sockfd);
    }
    int GetSockfd() {
        return sockfd;
    }

    void DurationTraceBegin();
    void DurationTraceEnd();
    void CompleteTrace();
    void InstantTrace();
    void CounterTrace();
    void AsyncTraceNestStart();
    void AsyncTraceNestEnd();
    void AsyncTraceNestInstant();
    void FlowTraceStart();
    void FlowTraceEnd();
    void FlowTraceStep();
    void SampleTrace();
    void ObjectTraceCreate();
    void ObjectTraceDestroy();
    void ObjectTraceSnapshot();
    void MetadataTrace();
    void MemoryDumpTraceGlobal();
    void MemoryDumpTraceProcess();
    void MarkTrace();
    void ClockSyncTrace();
private:
    cJSON* EventToJson(const ChromeTraceEvent& event, cJSON *root);
    void ReceiverThread();
    void WriterThread();

    std::map<uint32_t, const char*> trcNameMap;
    std::list<ChromeTraceEvent> eventList; // 使用std::list存储事件
    std::mutex eventMutex; // 用于互斥访问事件列表的互斥锁
    /*receiver*/
    std::thread receiverThread;
    std::thread writerThread;
    /*sender*/
    struct sockaddr_in serverAddr;
    int sockfd;
};