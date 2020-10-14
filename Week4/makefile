CC = gcc
CFLAGS = -Wall

all:
	make server && make client
server:
	$(CC) $(CFLAGS) -o server.o socket.c linkedList.c server.c
client:
	${CC} ${CFLAGS} -o client.o socket.c menu.c inputField.c client.c
run_server_default:
	./server.o 4444
run_client_default:
	./client.o 127.0.0.1 4444
delete:
	rm *.o