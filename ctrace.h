#pragma once

// Chrome Trace事件数据结构

struct ChromeTraceEvent {
    char name[128];
    char ph;
    int64_t ts;
    int pid;
    int tid;
};