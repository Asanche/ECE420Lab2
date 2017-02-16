all:clean slow fast 
slow: client mutexServer arrayServer
fast: client rwlServer

client: source/client.c
	gcc -g -Wall -std=c99 source/client.c -o client -lpthread

mutexServer: source/mutexServer.c
	gcc -g -Wall -std=c99 source/mutexServer.c -o mutexServer -lpthread

rwlServer: source/rwlServer.c
	gcc -g -Wall -std=gnu99 source/rwlServer.c -o rwlServer -lpthread

arrayServer: source/arrayServer.c
	gcc -g -Wall -std=c99 source/arrayServer.c -o arrayServer -lpthread

zip: readme.txt members.txt Makefile client.c server.c service.h
	zip 1393331-H42.zip readme.txt members.txt Makefile soure/client.c source/server.c source/service.h source/timer.h

clean:
	rm -rf *.o *.exe *.zip


