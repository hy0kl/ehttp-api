DEPEND_LIB = -lcurl \
		 -levent \
		 -levhtp \
		 -lzdb \
		 -lzlog \
		 -lhiredis

INCLUDEDIR = -I src/ \
			 -I contrib/cjson

CC = cc
CFLAGS = -g -Wall -Winline -pipe -fPIC

TARGET = api-server
OBJS = src/main.o contrib/cjson/cJSON.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(DEPEND_LIB) $(INCLUDEDIR) $<

#$(OBJS): src/main.c

#$.o: %c
#	$(CC) $(CCFLAGS) $(INCLUDEDIR) $(DEPEND_LIB) -c $< -o $@

clean:
	rm $(TARGET) $(OBJS)
