LIBDIR = -lcurl \
		 -levhtp \
		 -levent \
		 -lzlog \
		 -lhiredis

CC = cc
CFLAGS = -g -Wall -Winline -pipe -fPIC

CURLMEM = getinmemory
CURLMEM_OBJ = getinmemory.o

HTTP_SEVER = http-server
HTTP_SEVER_OBJ = http-server.o

EVHTP_BASE = evhtp_base
EVHTP_BASE_OBJ = evhtp_base.o

REDIS = credis
REDIS_OBJ = credis.o

ZLOG = czlog
ZLOG_BOJ = czlog.o

.PHONY: all clean

all: $(CURLMEM) $(HTTP_SEVER) $(EVHTP_BASE) $(REDIS) $(ZLOG)

$(CURLMEM): $(CURLMEM_OBJ)
	$(CC) -o $(CURLMEM) $(LIBDIR) $<

$(HTTP_SEVER): $(HTTP_SEVER_OBJ)
	$(CC) -o $(HTTP_SEVER) $(LIBDIR) $<

$(EVHTP_BASE): $(EVHTP_BASE_OBJ)
	$(CC) -o $(EVHTP_BASE) $(LIBDIR) $<

$(REDIS): $(REDIS_OBJ)
	$(CC) -o $(REDIS) $(LIBDIR) $<

$(ZLOG): $(ZLOG_BOJ)
	$(CC) -o $(ZLOG) $(LIBDIR) $<

#.c.o:
#	$(CC) -pedantic -c $(CFLAGS) $(INCLUDEDIR) $(LIBDIR) $< -o $@

clean:
	rm $(CURLMEM) $(CURLMEM_OBJ) $(HTTP_SEVER) $(HTTP_SEVER_OBJ) $(EVHTP_BASE) $(EVHTP_BASE_OBJ) $(REDIS) $(REDIS_OBJ)
