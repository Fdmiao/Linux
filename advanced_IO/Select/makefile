.PHONY:clean

.PHONY:all

all:tcp_server_select tcp_client
tcp_server_select:tcp_server_select.c
	gcc -o $@ -g $^

tcp_client:tcp_client.c
	gcc -o $@ -g $^

clean:
	rm tcp_server_select tcp_client
