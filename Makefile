DEPEND_LIB = -lcurl \
		 -levent \
		 -levhtp \
		 -lzdb \
		 -lzlog \
		 -lpthread \
		 -lm \
		 -lhiredis \
		 -lcrypto \
		 -ljemalloc

INCLUDEDIR = -I src/ \
			 -I src/account/ \
			 -I contrib/cjson/

CC = cc
CFLAGS = -g -Wall -Winline -pipe -fPIC

TARGET = api-server
OBJS = src/main.o src/util.o src/init.o \
	   src/crypto_wrap.o \
	   src/account/demo.o \
	   src/setproctitle.o \
	   contrib/cjson/cJSON.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $^ $(DEPEND_LIB) $(INCLUDEDIR)

%.o : %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm $(TARGET) $(OBJS)
