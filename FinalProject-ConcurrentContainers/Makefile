all: counter_main finalproject


finalproject: main.o MyTester.o Barriers.o SGL_stack.o Locks.o Treiber_stack.o SGL_queue.o MS_queue.o
	g++ main.o MyTester.o Barriers.o Locks.o SGL_stack.o Treiber_stack.o SGL_queue.o MS_queue.o -pthread -O0 -g -ofinalproject
	
main.o: main.cpp
	g++ -c -g main.cpp -pthread
	
MyTester.o: MyTester.cpp
	g++ -c -g MyTester.cpp -pthread
	
Barriers.o: Barriers.cpp
	g++ -c -g Barriers.cpp -pthread
	
Locks.o: Locks.cpp
	g++ -c -g Locks.cpp -pthread

MS_queue.o: MS_queue.cpp
	g++ -c -g MS_queue.cpp -pthread

SGL_queue.o: SGL_queue.cpp
	g++ -c -g SGL_queue.cpp -pthread

SGL_stack.o: SGL_stack.cpp
	g++ -c -g SGL_stack.cpp -pthread

Treiber_stack.o: Treiber_stack.cpp
	g++ -c -g Treiber_stack.cpp -pthread
	
counter_main: counter.c
	gcc counter.c -pthread -O0 -g -o counter_main
	
clean:
	rm *.o finalproject counter_main main.o MyTester.o Barriers.o Locks.o SGL_stack.o Treiber_stack.o SGL_queue.o MS_queue.o