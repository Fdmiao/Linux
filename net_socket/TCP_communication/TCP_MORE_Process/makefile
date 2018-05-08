.PHONY:all

all:tcp_server tcp_client

tcp_server:tcp_server.c
	gcc -o $@ $^

tcp_client:tcp_client.c
	gcc -o $@  $^ -static

.PHONY:clean

clean:
	rm tcp_server tcp_client
