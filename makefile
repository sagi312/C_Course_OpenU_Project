FLAGS = -Wall -ansi -pedantic

assembler: assembler.o table.o inputOutput.o preassembler.o firstPass.o typeChecker.o secondPass.o encoder.o converter.o stringUtils.o
	gcc -o assembler $(FLAGS) $^ -lm 

assembler.o: assembler.c config.h table.h inputOutput.h converter.h preassembler.h firstPass.h secondPass.h
	gcc -c assembler.c $(FLAGS) 

preassembler.o: preassembler.c preassembler.h table.h inputOutput.h stringUtils.h converter.h config.h
	gcc -c preassembler.c $(FLAGS)

table.o: table.c table.h stringUtils.h
	gcc -c table.c $(FLAGS) 

inputOutput.o: inputOutput.c inputOutput.h table.h stringUtils.h
	gcc -c inputOutput.c $(FLAGS)

typeChecker.o: typeChecker.c table.h inputOutput.h typeChecker.h config.h
	gcc -c typeChecker.c $(FLAGS) 

firstPass.o: firstPass.c firstPass.h table.h inputOutput.h typeChecker.h encoder.h converter.h config.h
	gcc -c firstPass.c $(FLAGS) 

secondPass.o: secondPass.c secondPass.h table.h inputOutput.h typeChecker.h config.h
	gcc -c secondPass.c $(FLAGS) 

encoder.o: encoder.c encoder.h table.h inputOutput.h typeChecker.h converter.h config.h
	gcc -c encoder.c $(FLAGS)

converter.o: converter.c converter.h inputOutput.h config.h
	gcc -c converter.c $(FLAGS)

stringUtils.o: stringUtils.c stringUtils.h
	gcc -c stringUtils.c $(FLAGS) 

clean:
	rm *.o *.ob *.ent *.am *.ext assembler