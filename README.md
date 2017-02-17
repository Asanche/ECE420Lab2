#README
##Respository for ECE420 Lab 2

###CONTENTS
* Compilation Instructions
* Makefile commands
* General Descriptions

####DESCRIPTION

We have three separate implementations of a multithreaded server using pthreads. We made one where
there is a single mutex that controls ALL access to the array, be they reads or writes. We made one
that utilizes an array of mutexes the same length as the array of strings (one per string) to control 
read and write access (inclusive) to each array element individually. We also used the pthreads_rwl (read write lock)
in GNU99 to control reads and writes to the array separately. In this case, reads get top priority, and infinite reads 
can occur simultaneously, with one possible write at a time, and no reads while writing.

####COMPILATION
* "**" denotes general purpose compilation of a group of files - TAs Use these Please
* "*" denotes commands used specifically for the purpose of the lab.
##### Make commands 
- ** make all - makes the printing and non printing versions of the fast and slow servers
- ** make slow - makes the slow versions of the client and server
- ** make fast - makes the fast versions of the client and server
======================================
- * make client - makes the client binary
- * make rwlServer - makes server that uses a read write lock
- * make arrayServer - makes server that has an array of mutexes (one per element)
- * make mutexServer - makes server that has single mutex
======================================
- make clean - removes executables, binaries, compressed folders
- make zip - makes a zip of the required submission files
- make test - clears results and runs test.sh

#####RUN INSTRUCTIONS
- ./client <port> <arraySize> - runs the client
- ./mutexServer <port> <arraySize> - runs the single mutex server
- ./arrayServer <port> <arraySize> - runs the array of mutexes server
- ./arrayServer <port> <arraySize> - runs the read write lock server