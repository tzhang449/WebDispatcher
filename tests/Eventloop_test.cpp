#include <cstdio>
#include <thread>

#include "Eventloop.h"

void threadFunc(){
    printf("threadFunc started");
    
    Eventloop loop;
    loop.loop();
}

int main(){
    printf("main started");

    Eventloop loop;

    std::thread thrd(threadFunc);

    loop.loop();

    thrd.join();   
}