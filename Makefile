CC := g++

%.o:%.cpp
	$(CC) -c $^ -o $@

.PHONY:clean

clean:
	rm *.o