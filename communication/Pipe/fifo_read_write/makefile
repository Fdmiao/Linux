.PHONY:all
all:read_fifo write_fifo
read_fifo:read_fifo.c
	gcc -o $@ $^

write_fifo:write_fifo.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm read_fifo write_fifo
