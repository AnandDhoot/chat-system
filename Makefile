.PHONY: clean

all: client server1 server2

client: client.c
	@gcc -o client client.c

server1: server1.c
	@gcc -o server1 server1.c

server2: server2.c
	@gcc -o server2 server2.c

clean:
	@rm -f *~ server? client
