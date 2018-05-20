.PHONY:all

all:add_server add_client

add_server:add_server.c 
	gcc -o $@ $^

add_client:add_client.c
	gcc -o $@  $^ -static

.PHONY:clean

clean:
	rm add_server add_client
