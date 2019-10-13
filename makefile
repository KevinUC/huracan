sshell: sshell.o Task.o Helpers.o Mylibrary.o List.o
	gcc -Wall -Werror -o sshell sshell.o Task.o Helpers.o Mylibrary.o List.o

sshell.o: sshell.c Task.h Helpers.h List.h
	gcc -Wall -Werror -c sshell.c

Helpers.o: Helpers.c Task.h Mylibrary.h List.h
	gcc -Wall -Werror -c Helpers.c

Task.o: Task.c
	gcc -Wall -Werror -c Task.c

Mylibrary.o: Mylibrary.c
	gcc -Wall -Werror -c Mylibrary.c

List.o: List.c Task.h Helpers.h
	gcc -Wall -Werror -c List.c

clean:
	rm sshell *.o
