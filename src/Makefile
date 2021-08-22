CC=gcc
CFLAGS=-I/usr/include/libnl3 -lnl-genl-3 -lnl-3 -lpthread -Wno-implicit-function-declaration  

all: mptcp_nl_server mptcp_nl_client

mptcp_nl_server: ./server/mptcp_nl_server.o ./func/cmd_func.o ./func/event_func.o
	$(CC) -o $@ $< $(CFLAGS)

mptcp_nl_client: ./client/mptcp_nl_client.o ./func/cmd_func.o ./func/event_func.o
	$(CC) -o $@ $< $(CFLAGS)

./server/mptcp_nl_server.o: ./server/mptcp_nl_server.c
	$(CC) -c -o $@ $< $(CFLAGS)

./client/mptcp_nl_client.o: ./client/mptcp_nl_client.c
	$(CC) -c -o $@ $< $(CFLAGS)

./func/cmd_func.o: ./func/cmd_func.c
	$(CC) -c -o $@ $< $(CFLAGS)

./func/event_func.o: ./func/event_func.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm mptcp_nl_server
	rm mptcp_nl_client
	rm ./func/*.o
	rm ./server/*.o
	rm ./client/*.o
