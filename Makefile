all:clean slow fast
slow: client server
fast: client fastServer

client: source/client.c
	gcc -g -Wall -std=c99 source/client.c -o client -lpthread

server: source/server.c
	gcc -g -Wall -std=c99 source/server.c -o server -lpthread

fastServer: source/fastServer.c
	gcc -g -Wall -std=c99 source/fastServer.c -o fastServer -lpthread

zip: readme.txt members.txt Makefile client.c server.c service.h
	zip 1393331-H42.zip readme.txt members.txt Makefile soure/client.c source/server.c source/service.h source/timer.h

clean:
	rm -rf *.o *.exe *.zip


