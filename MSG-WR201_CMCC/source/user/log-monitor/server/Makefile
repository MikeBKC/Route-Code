# for ReOSSE, delete CC and CFLAGS definition
CC = gcc
CFLAGS = 
SERVER = logMonitor
OBJS_SRV = server.o
OBJS = md5.o aes.o

.PHONY: all clean

all: $(SERVER)

$(SERVER): $(OBJS_SRV) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS_SRV) $(OBJS)

server.o:
	$(CC) $(CFLAGS) -c server.c -o server.o

md5.o:
	$(CC) $(CFLAGS) -c md5.c -o md5.o

aes.o:
	$(CC) $(CFLAGS) -c aes.c -o aes.o

clean:
	rm -rf $(SERVER) $(OBJS_SRV) $(OBJS)
