CC		= clang
CFLAGS	= 

OBJS	= main.o vector.o token.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

default:
	make scc

test: scc
	./test.sh

scc: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
