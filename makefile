all:
	gcc -pthread tours.c wrappers.c -o tours
clean:
	rm -f *.o tours
