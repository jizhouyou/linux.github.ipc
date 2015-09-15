CC=gcc
CFLAGS=-Wall -g
Target=Client_p2p Server_p2p
all:$(Target)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY:clean all
clean:
	rm -f *.o $(Target)
