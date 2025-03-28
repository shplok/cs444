CC=cc
CFLAGS=-Wall -O2

all: raidsbb diarsbb

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f a.out *.part? *.2
