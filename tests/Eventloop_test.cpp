#include <cstdio>
#include <thread>

#include "Eventloop.h"

//This test tests the behavior of Eventloop, Channel and Epoller

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
    loop.quit();
}