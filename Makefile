CC=gcc
CFLAGS=-Wall -g
BIN=miniftpd
OBJS=main.o sysutil.o session.o str.o tunable.o \
	 parseconf.o ftpproto.o ftpnobody.o ftpipc.o \
	 hash.o test.o clntlmt.o
LIBS=-lcrypt
.PHONY:clean TAGS
$(BIN):$(OBJS)
	$(CC) $(CFLAGS)	-o $@ $^ $(LIBS)
TAGS:
	ctags -R *
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN) $(OBJS)

