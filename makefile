FLAGS = -Wall -ansi -pedantic -g

assembler: assembler.o table.o inputOutput.o preassembler.o firstPass.o
	gcc -o assembler $(FLAGS) $^ -lm 

assembler.o: assembler.c table.h inputOutput.h preassembler.h firstPass.h
	gcc -c assembler.c $(FLAGS)

preassembler.o: preassembler.c table.h inputOutput.h
	gcc -c preassembler.c $(FLAGS)

table.o: table.c table.h
	gcc -c table.c $(FLAGS)

inputOutput.o: inputOutput.c inputOutput.h table.h
	gcc -c inputOutput.c $(FLAGS)

firstPass.o: firstPass.c firstPass.h table.h inputOutput.h
	gcc -c firstPass.c $(FLAGS)

clean:
	rm *.o assembler