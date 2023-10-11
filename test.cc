#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ctrace.h"
#include <iostream>

int main() {


    ChromeTrace ctrace;
    long idx=0;

    int sockfd = ctrace.GetSockfd();
    for(int i=1; i<10;i++) {
        // 创建一个Chrome Trace事件并发送
        ctrace.DurationTraceBegin(0, 0, B, 500*i+100, 0);    
        ctrace.DurationTraceEnd(0, 0, B, 500*i+250, 0);
        ctrace.CompleteTrace(0,0,C, 500*i, 500, 0);
        ctrace.InstantTrace(0,2, A, 500*i +150, 'p', 0);

        ctrace.DurationTraceBegin(0, 2, A, 500*i+300, 1);
        ctrace.DurationTraceEnd(0, 2, A, 500*i+480, 2);

        std::cout << "Sending " << idx++<<" events"<<std::endl;
    }
    while(1){;}

    return 0;
}
