all:
	gcc -pthread server.c -o server
	gcc client.c -o getfile

server:
	gcc -pthread server.c -o server

client:
	gcc client.c -o client

clean:
	rm -f server client
