FLAGS = -Wall -ansi -pedantic

assembler: assembler.o preassembler.o table.o
	gcc -o assembler $(FLAGS) $^ -lm 

assembler.o: assembler.c table.h
	gcc -c assembler.c $(FLAGS)

preassembler.o: preassembler.c table.h
	gcc -c preassembler.c $(FLAGS)

table.o: table.c table.h
	gcc -c table.c $(FLAGS)

clean:
	rm *.o assembler