all:clean slow fast noP noPFast
slow: client server
fast: client fastServer
noP: noPClient noPServer
noPFast: noPClient noPFastServer

client: multiThreadedClient.c  multiThreadedCS.h
	gcc -std=c99 multiThreadedClient.c multiThreadedCS.h -o client -lpthread

server: multiThreadedServer.c multiThreadedCS.h
	gcc -std=c99 multiThreadedServer.c multiThreadedCS.h -o server -lpthread

fastServer: fastMultiThreadedServer.c multiThreadedCS.h
	gcc -std=c99 fastMultiThreadedServer.c multiThreadedCS.h -o fastServer -lpthread

noPClient: multiThreadedClient.c  multiThreadedCS.h
	gcc -std=c99 noPrinting/multiThreadedClient.c multiThreadedCS.h -o noPClient -lpthread

noPServer: multiThreadedServer.c multiThreadedCS.h
	gcc -std=c99 noPrinting/multiThreadedServer.c multiThreadedCS.h -o noPServer -lpthread

noPFastServer: fastMultiThreadedServer.c multiThreadedCS.h
	gcc -std=c99 noPrinting/fastMultiThreadedServer.c multiThreadedCS.h -o noPFastServer -lpthread

zip: readme.txt members.txt Makefile multiThreadedClient.c multiThreadedServer.c multiThreadedCS.h
	zip 1393331-H42.zip readme.txt members.txt Makefile multiThreadedClient.c multiThreadedServer.c multiThreadedCS.h

clean:
	rm -rf *.o *.exe *.zip


