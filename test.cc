#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ctrace.h"
#include <iostream>
#include <string>

int main() {


    ChromeTrace ctrace(false);
    long idx=0;
    std::string dur = "duration";
    std::string cplt = "complete";
    std::string instant = "instant";
    std::string counter = "counter";
    std::string async = "async";
    for(int i=1; i<5;i++) {
        // 创建一个Chrome Trace事件并发送
        ctrace.DurationTraceBegin(0, 0, dur.c_str(), 500*i+100, 0);    
        ctrace.DurationTraceEnd(0, 0, dur.c_str(), 500*i+250, 0);
        ctrace.CompleteTrace(0,0,cplt.c_str(), 500*i, 500, 0);
        ctrace.InstantTrace(0,2, instant.c_str(), 500*i +150, 't', 0);
        ctrace.CounterTrace(0,1,counter.c_str(),500*i+10,0);
        ctrace.AsyncTraceNestStart(0,0,async.c_str(),500*i+20,0);
        ctrace.AsyncTraceNestEnd(0,0,async.c_str(),500*i+50,0);
        ctrace.AsyncTraceNestInstant(0,0,async.c_str(),500*i+30,0);
        ctrace.DurationTraceBegin(0, 2, dur.c_str(), 500*i+300, 1);
        ctrace.DurationTraceEnd(0, 2, dur.c_str(), 500*i+480, 2);

        std::cout << "Sending " << idx++<<" events"<<std::endl;
    }
    while(1){;}

    return 0;
}
