compile: linear_sequence.o main.o 
	gcc linear_sequence.o main.o -o test
	rm *.o  
liner_.o: linear_sequence.c linear_sequence.h
	gcc -c linear_sequence.c ./libdmalloc.a
main.o: main.c linear_sequence.h
	gcc -c main.c
clear:
	rm *.o cp


