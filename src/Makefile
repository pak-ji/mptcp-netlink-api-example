CC=gcc
CFLAGS=-I/usr/include/libnl3 -lnl-genl-3 -lnl-3 -lpthread -Wno-implicit-function-declaration  

all: mptcp_nl_server mptcp_nl_client

mptcp_nl_server: ./server/mptcp_nl_server.o ./func/cmd_func.o ./func/event_func.o
	$(CC) -o $@ $< $(CFLAGS)

mptcp_nl_client: ./client/mptcp_nl_client.o ./func/cmd_func.o ./func/event_func.o
	$(CC) -o $@ $< $(CFLAGS)

./server/mptcp_nl_server.o: ./server/mptcp_nl_server.c
	$(CC) -c $@ $< $(CFLAGS)

./client/mptcp_nl_client.o: ./client/mptcp_nl_client.c
	$(CC) -c $@ $< $(CFLAGS)

clean:

# 테스트해보며 계속해서 작성..
