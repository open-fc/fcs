src = $(wildcard *.c b36/*.c)
obj = $(patsubst %.c,%.o,$(src))
arg = -lpthread -lhiredis -luuid -lcurses -Wall -g -lwiringPi

name= fcs

all:$(obj)
	gcc $^ $(arg) -o ../$(name)

%.o:%.c
	gcc -c $< -o $@ $(arg) 

clean:
	-rm -rf $(obj) ../$(name)

.phony:clean all
