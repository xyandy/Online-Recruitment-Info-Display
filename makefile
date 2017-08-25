ROOT_PATH=$(shell pwd)
WWWROOT=$(ROOT_PATH)/wwwroot

FLAGS=-D_DEBUG_
LDFLAGS=-lpthread

bin=http
src=http.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g')

$(bin):$(obj)
	gcc -o $@ $^ $(LDFLAGS) 
%.o:%.c
	gcc -c $< 

.PHONY:clean
clean:
	rm -f *.o $(bin)
