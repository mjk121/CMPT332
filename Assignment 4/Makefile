all: server sender receiver

server: server.c
	gcc -Wall -Wextra -g server.c -o server

sender: sender.c
	gcc -Wall -Wextra -g sender.c -o sender

receiver: receiver.c
	gcc -Wall -Wextra -g receiver.c -o receiver
	
clean:
	rm -f server sender receiver *.o *~
