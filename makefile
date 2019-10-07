sshell: sshell.o Task.o Helpers.o Mylibrary.o
	gcc -Wall -Werror -o sshell sshell.o Task.o Helpers.o Mylibrary.o

sshell.o: sshell.c Task.h Helpers.h
	gcc -Wall -Werror -c sshell.c

Helpers.o: Helpers.c Task.h Mylibrary.h
	gcc -Wall -Werror -c Helpers.c

Task.o: Task.c
	gcc -Wall -Werror -c Task.c

Mylibrary.o: Mylibrary.c
	gcc -Wall -Werror -c Mylibrary.c

clean:
	rm sshell *.o
