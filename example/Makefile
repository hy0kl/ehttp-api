LIBDIR = -lcurl

CC = cc
CFLAGS = -g -Wall -Winline -pipe -fPIC

CURLMEM = getinmemory
CURLMEM_OBJ = getinmemory.o

.PHONY: all clean

all: $(CURLMEM)

$(CURLMEM): $(CURLMEM_OBJ)
	$(CC) -o $(CURLMEM) $(LIBDIR) $<

clean:
	rm $(CURLMEM) $(CURLMEM_OBJ)
