FLAGS = -Wall -ansi -pedantic -g

assembler: assembler.o table.o inputOutput.o preassembler.o firstPass.o typeChecker.o secondPass.o encoder.o converter.o
	gcc -o assembler $(FLAGS) $^ -lm 2> mainOutput.txt

assembler.o: assembler.c table.h inputOutput.h preassembler.h firstPass.h typeChecker.h secondPass.h converter.h config.h
	gcc -c assembler.c $(FLAGS) 2> assemblerOutput.txt

preassembler.o: preassembler.c preassembler.h table.h inputOutput.h config.h 
	gcc -c preassembler.c $(FLAGS) 2> preassemblerOutput.txt

table.o: table.c table.h
	gcc -c table.c $(FLAGS) 2> tableOutput.txt

inputOutput.o: inputOutput.c inputOutput.h table.h
	gcc -c inputOutput.c $(FLAGS) 2> inputOutputOutput.txt

typeChecker.o: typeChecker.c table.h inputOutput.h typeChecker.h config.h
	gcc -c typeChecker.c $(FLAGS) 2> typeCheckerOutput.txt

firstPass.o: firstPass.c firstPass.h table.h inputOutput.h typeChecker.h encoder.h converter.h config.h
	gcc -c firstPass.c $(FLAGS) 2> firstPassOutput.txt

secondPass.o: secondPass.c secondPass.h table.h inputOutput.h typeChecker.h config.h
	gcc -c secondPass.c $(FLAGS) 2> secondPassOutput.txt

encoder.o: encoder.c encoder.h table.h inputOutput.h typeChecker.h converter.h config.h
	gcc -c encoder.c $(FLAGS) 2> encoderOutput.txt

converter.o: converter.c converter.h inputOutput.h config.h
	gcc -c converter.c $(FLAGS) 2> converterOutput.txt

clean:
	rm *.o assembler