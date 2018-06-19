
.PHONY:all

all:tcp_client ex_tcp_server_epoll_et

tcp_client:tcp_client.c
	gcc -o $@ -g $^

ex_tcp_server_epoll_et:ex_tcp_server_epoll_et.c
	gcc -o $@ -g $^

.PHONY:clean

clean:
	rm ex_tcp_server_epoll_et tcp_client

