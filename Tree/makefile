compile: linear_sequence_assoc.o main.o 
	gcc linear_sequence_assoc.o main.o -o test 
linear_sequence_assoc.o: 
	gcc -c linear_sequence_assoc.c  linear_sequence_assoc.h 
main.o: main.c linear_sequence_assoc.h
	gcc -c main.c
clear:
	rm *.o cp


