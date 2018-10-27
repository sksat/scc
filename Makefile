CC		= clang
CFLAGS	= 

OBJS	= main.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

default:
	make scc

scc: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
