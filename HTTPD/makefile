
ROOT=$(shell pwd)

.PHONY:all
	
all:httpd cgi

httpd:httpd.c
	gcc -o $@ -g $^ -lpthread

cgi:
	cd $(ROOT)/wwwroot/cgi; make clean; make; cd -


.PHONY:output

output:
	mkdir -p output/wwwroot/cgi
	cp httpd output
	cp start.sh output
	cp -rf lib output
	cp -rf wwwroot/*.html output/wwwroot
	cp -rf wwwroot/img/ output/wwwroot
	cp -rf wwwroot/cgi/netCal output/wwwroot/cgi
	cp -rf wwwroot/cgi/insertData output/wwwroot/cgi
	cp -rf wwwroot/cgi/selectData output/wwwroot/cgi


.PHONY:clean

clean:
	rm httpd; cd $(ROOT)/wwwroot/cgi; make clean; cd -
	rm -rf output


