all:clean slow fast 
slow: client mutexServer arrayServer
fast: client rwlServer
test: testClean testsh
testsh:
	./test.sh
testClean:
	rm -rf results/*.txt

client: source/client.c
	gcc -g -Wall -std=c99 source/client.c -o client -lpthread

mutexServer: source/mutexServer.c
	gcc -g -Wall -std=c99 source/mutexServer.c -o mutexServer -lpthread

rwlServer: source/rwlServer.c
	gcc -g -Wall -std=gnu99 source/rwlServer.c -o rwlServer -lpthread

arrayServer: source/arrayServer.c
	gcc -g -Wall -std=c99 source/arrayServer.c -o arrayServer -lpthread

zip: clean
	zip 1393331-H42.zip source/readme.txt members.txt Makefile source/client.c source/arrayServer.c source/service.h source/timer.h source/rwlServer.c source/mutexServer.c

clean:
	rm -rf *.o *.exe *.zip


