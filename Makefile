all:
	gcc -Wall -c src/commom.c
	gcc -Wall src/client.c commom.o -o client
	gcc -Wall src/server.c commom.o -o server