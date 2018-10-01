438 Time-line:

9-29:

server - generate_http_request() finished

given filename it can read the file and write it to buffer and compose a response string



9-31:

server - multi-process enabled, cycling send and recv 

server - debugged

clinet - input parser
parse standard input like https://xxx.x.x.x:xx/xxx

client - input parse debugged

client - cycling send and recv(not debugged )


TO-DO: 


1. debug client.c

2. modify the input standard of server and client to matah the requirements of autograder.  

3. makefile 


[test]:

1. put [input] and [server.c] in same folder and compile. -vm1

2. put [client.c] in a file and compile.  -vm2

3. start server.exe in vm1 [./server ]

4. start client.exe in vm2 [./client http://hostname[:port]/input]

5. expected result: ./input in vm1 is the exactly the same as ./input vm2

