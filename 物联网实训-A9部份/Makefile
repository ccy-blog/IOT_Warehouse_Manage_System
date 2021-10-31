#CC=gcc
CC=arm-none-linux-gnueabi-gcc
AFLAGS= -c -g
LDFLAGS= -lpthread # -L ./lib -lsqlite3 -I./include
OBJS=main.o uart.o pthread_transfer.o pthread_analysis.o pthread_refresh.o sem.o pthread_client_request.o

main :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	
$(OBJS):%.o:%.c
	$(CC) $(AFLAGS) $< -o $@

.PHONY:clean
clean:
	rm *.o main

cp:
	sudo cp main /source/rootfs/
