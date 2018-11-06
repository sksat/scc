CC		= clang
CFLAGS	= 

OBJS	= main.o vector.o token.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: default test

default:
	make scc

clean:
	rm -f scc *.o

test: scc
	./test.sh

scc: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
