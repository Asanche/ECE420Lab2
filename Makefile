client: multiThreadedClient.c  multiThreadedCS.h
	gcc -std=c99 multiThreadedClient.c multiThreadedCS.h -o client -lpthread

server: multiThreadedServer.c multiThreadedCS.h
	gcc -std=c99 multiThreadedServer.c multiThreadedCS.h -o server -lpthread

zip: readme.txt members.txt Makefile 
	zip 1393331-H42.zip readme.txt members.txt Makefile


