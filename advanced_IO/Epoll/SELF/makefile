
.PHONY:all

all:tcp_client write_self

tcp_client:tcp_client.c
	gcc -o $@ -g $^

write_self:write_self.c
	gcc -o $@ -g $^

.PHONY:clean

clean:
	rm tcp_client write_self
