vpath %.cpp ..
CC := g++
CFLAGS := -g -Wall -std=c++14 -pthread

%.o:%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@ -I..

Logger_test:Logger_test.o Logger.o
	$(CC) $(CFLAGS)  $^ -o $@ -I..
	rm *.o

Eventloop_test:Eventloop_test.o Eventloop.o Channel.o Epoller.o Logger.o 
	$(CC) $(CFLAGS)  $^ -o $@ -I..
	rm *.o

Acceptor_test:Acceptor_test.o Eventloop.o Channel.o Epoller.o Acceptor.o Logger.o
	$(CC) $(CFLAGS)  $^ -o $@ -I..
	rm *.o

Server_test:Server_test.o Eventloop.o Channel.o Epoller.o Acceptor.o Logger.o LoopThread.o ThreadPool.o Server.o Connection.o
	$(CC) $(CFLAGS)  $^ -o $@ -I..
	rm *.o

.PHONY: clean 
.PHONY: cleanlog

clean:
	rm Logger_test Eventloop_test Acceptor_test Server_test *.o *.log

cleanlog:
	rm *.log