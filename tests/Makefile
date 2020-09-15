vpath %.cpp ..
CC := g++
CFLAGS := -g -Wall -std=c++14 -pthread

%.o:%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@ -I..

Logger_test:Logger_test.o Logger.o
	$(CC) $(CFLAGS)  $^ -o $@ -I..
	rm *.o

.PHONY: clean

clean:
	rm Logger_test *.log