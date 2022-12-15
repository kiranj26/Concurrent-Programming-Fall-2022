all: counter mysort
	
counter: counter.c
	gcc counter.c -pthread -O0 -g -o counter

mysort: mysort.c
	gcc mysort.c -pthread -O0 -g -o mysort

.PHONY : clean
clean:
	rm counter mysort
