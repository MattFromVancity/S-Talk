CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200112L -pthread -Wall -Werror
CC = cc

build: stalk.o list.o receive.o transmit.o user_input.o user_output.o cleaner.o stalk_types.o
	$(CC) $(CFLAGS) -o s-talk stalk.o list.o receive.o transmit.o user_input.o user_output.o cleaner.o stalk_types.o

stalk.o: stalk.c
	$(CC) $(CFLAGS) -c stalk.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

receive.o: receive.c
	$(CC) $(CFLAGS) -c receive.c

transmit.o : transmit.c
	$(CC) $(CFLAGS) -c transmit.c

user_input.o: user_input.c
	$(CC) $(CFLAGS) -c user_input.c

user_output.o: user_output.c
	$(CC) $(CFLAGS) -c user_output.c

cleaner.o: cleaner.c
	$(CC) $(CFLAGS) -c cleaner.c

stalk_types.o: stalk_types.c 
	$(CC) $(CFLAGS) -c stalk_types.c

clean:
	rm -f *.o s-talk
