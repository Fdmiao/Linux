.PHONY:all

all:tcp_server tcp_client

tcp_server:tcp_server.c comm.c
	gcc -o $@ -g $^

tcp_client:tcp_client.c comm.c
	gcc -o $@ -g $^ -static

.PHONY:clean

clean:
	rm tcp_server tcp_client
