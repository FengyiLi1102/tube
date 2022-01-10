tube: tube.o main.o
	g++ -Wall -g tube.o main.o -o tube

main.o: main.cpp
	g++ -Wall -g -c main.cpp

tube.o: tube.cpp tube.h
	g++ -Wall -g -c tube.cpp

clean:
	rm -rf *.o tube