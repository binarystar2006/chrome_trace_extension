#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ctrace.h"
#include <iostream>

int main() {


    ChromeTrace ctrace(false);
    long idx=0;

    for(int i=1; i<5;i++) {
        // 创建一个Chrome Trace事件并发送
        ctrace.DurationTraceBegin(0, 0, 0, 500*i+100, 0);    
        ctrace.DurationTraceEnd(0, 0, 0, 500*i+250, 0);
        ctrace.CompleteTrace(0,0,3, 500*i, 500, 0);
        ctrace.InstantTrace(0,2, 1, 500*i +150, 't', 0);
        ctrace.CounterTrace(0,1,2,500*i+10,0);
        ctrace.AsyncTraceNestStart(0,0,4,500*i+20,0);
        ctrace.AsyncTraceNestEnd(0,0,4,500*i+50,0);
        ctrace.AsyncTraceNestInstant(0,0,4,500*i+30,0);
        ctrace.DurationTraceBegin(0, 2, 0, 500*i+300, 1);
        ctrace.DurationTraceEnd(0, 2, 0, 500*i+480, 2);

        std::cout << "Sending " << idx++<<" events"<<std::endl;
    }
    while(1){;}

    return 0;
}
