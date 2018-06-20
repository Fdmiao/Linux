.PHONY:all

all:tcp_client tcp_server_poll

tcp_client:tcp_client.c
	gcc -o $@ -g $^

tcp_server_poll:tcp_server_poll.c
	gcc -o $@ -g $^

.PHONY:clean

clean:
	rm tcp_server_poll tcp_client
