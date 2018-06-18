

ROOT=$(shell pwd)
INCLUDE=-I$(ROOT)/../../lib/include
LIB=-L$(ROOT)/../../lib/lib
LIB_NAME=-lmysqlclient


.PHONY:all

all:netCal insertData selectData


netCal:netCal.c
	gcc -o $@ -g $^

insertData:insertData.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)

selectData:selectData.c
	gcc -o $@ -g $^

.PHONY:clean

clean:
	rm netCal insertData selectData
