all:clean slow fast noP noPFast
slow: client server
fast: client fastServer
noP: noPClient noPServer
noPFast: noPClient noPFastServer

client: client.c  service.h
	gcc -std=c99 client.c service.h -o client -lpthread

server: server.c service.h
	gcc -std=c99 server.c service.h -o server -lpthread

fastServer: fastServer.c service.h
	gcc -std=c99 fastServer.c service.h -o fastServer -lpthread

noPClient: client.c  service.h
	gcc -std=c99 noPrinting/client.c service.h -o noPClient -lpthread

noPServer: server.c service.h
	gcc -std=c99 noPrinting/server.c service.h -o noPServer -lpthread

noPFastServer: fastServer.c service.h
	gcc -std=c99 noPrinting/fastServer.c service.h -o noPFastServer -lpthread

zip: readme.txt members.txt Makefile client.c server.c service.h
	zip 1393331-H42.zip readme.txt members.txt Makefile client.c server.c service.h

clean:
	rm -rf *.o *.exe *.zip


