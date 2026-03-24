FOR TASK 2 HELLO APP

On the server VM, compile/run the server program:

gcc -Wall -Wextra -std=c11 hello_server.c -o hello_server
./hello_server 45678
On the client VM, compile/run the client program:

gcc -Wall -Wextra -std=c11 hello_client.c -o hello_client
./hello_client <SERVER_VM_IP> 45678
Use the same port on both sides. The client uses the server VM’s IP, not 127.0.0.1.
