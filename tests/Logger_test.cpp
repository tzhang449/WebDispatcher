#include <vector>
#include <thread>
#include <iostream>
#include <chrono>

#include"Logger.h"

//This test is not for the benchmark.
const int thread_num=10;
const int size=1000;
const int total=thread_num*size;

void threadFunc(){
    for(int i=0;i<size;i++){
        LOG_INFO("I writes a message for the %d time",i);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

int main(){
    std::vector<std::thread> vec;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for(int i=0;i<thread_num;i++){
        vec.push_back(std::thread(threadFunc));
    }
    for(auto &thread:vec){
        thread.join();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/total << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()/total << "[ns]" << std::endl;
}
