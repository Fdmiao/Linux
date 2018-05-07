


.PHONY:all

all:udp_server udp_client

udp_server:udp_server.c
	gcc -o $@ -g $^

udp_client:udp_client.c
	gcc -o $@ -g $^ -static 

.PHONY:clean

clean:
	rm udp_server udp_client
