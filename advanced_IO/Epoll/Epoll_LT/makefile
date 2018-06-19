.PHONY:all

all:tcp_client tcp_server_epoll

tcp_client:tcp_client.c
	gcc -o $@ -g $^

tcp_server_epoll:tcp_server_epoll.c
	gcc -o $@ -g $^

.PHONY:clean

clean:
	rm tcp_server_epoll tcp_client
