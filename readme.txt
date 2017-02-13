==== README ===
Readme file for ECE lab 420

=== CONTENTS ===
- Compilation Instructions
    - Makefile commands
- General Descriptions

=== DESCRIPTION ===

=== COMPILATION ===
** denotes general purpose compilation of a group of files - TAs Use these Please
* denotes commands used specifically for the purpose of the lab.
== Make commands ==
    - ** make all - makes the printing and non printing versions of the fast and slow servers
    - ** make slow - makes the slow versions of the client and server
    - ** make fast - makes the fast versions of the client and server
    - ** make noP - makes the slow versions of the client and server without printing
    - ** make noPFast = makes the fast versions of the client and server without printing
===
    - * make server - makes the server binary
    - * make client - makes the client binary
    - * make fastServer - makes the fast server binary
    - * make noPClient - makes the client binary with no printf
    - * make noPServer - makes the server binary with no printf
    - * make noPFastServer - makes the fast server binary with no printf
    
===
    - make clean - removes executables, binaries, compressed folders
    - make zip - makes a zip of the required submission files

=== RUN INSTRUCTIONS ===
- ./client <port> <arraySize> - runs the client
- ./server <port> <arraySize> - runs the server
- ./fastServer <port> <arraySize> - runs the faster server
 == Versions with no printing ==
- ./noPClient <port> <arraySize> - runs the client
- ./noPServer <port> <arraySize> - runs the server
- ./noPFastServer <port> <arraySize> - runs the faster server