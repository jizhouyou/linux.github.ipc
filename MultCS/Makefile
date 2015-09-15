CC=gcc
CFLAGS=-Wall -g
Target=Server_CS Client_CS
all:$(Target)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY:clean all
clean:
	rm -f *.o $(Target)
