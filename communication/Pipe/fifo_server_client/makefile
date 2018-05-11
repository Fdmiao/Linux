.PHONY:all
all:serverPipe clientPipe

serverPipe:serverPipe.c
	gcc -o $@ $^

clientPipe:clientPipe.c
	gcc -o $@ $^

.PHONY:clean
clean:
	rm serverPipe clientPipe
