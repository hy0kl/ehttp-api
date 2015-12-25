LIBDIR = -lcurl \
		 -levhtp \
		 -levent

CC = cc
CFLAGS = -g -Wall -Winline -pipe -fPIC

CURLMEM = getinmemory
CURLMEM_OBJ = getinmemory.o

HTTP_SEVER = http-server
HTTP_SEVER_OBJ = http-server.o

EVHTP_BASE = evhtp-base
EVHTP_BASE_OBJ = evhtp_base.o

.PHONY: all clean

all: $(CURLMEM) $(HTTP_SEVER)

$(CURLMEM): $(CURLMEM_OBJ)
	$(CC) -o $(CURLMEM) $(LIBDIR) $<

$(HTTP_SEVER): $(HTTP_SEVER_OBJ)
	$(CC) -o $(HTTP_SEVER) $(LIBDIR) $<

$(EVHTP_BASE): $(EVHTP_BASE_OBJ)
	$(CC) -o $(EVHTP_BASE) $(LIBDIR) $<

clean:
	rm $(CURLMEM) $(CURLMEM_OBJ) $(HTTP_SEVER) $(HTTP_SEVER_OBJ)
