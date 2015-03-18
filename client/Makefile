CC=g++


C_SOURCES = $(wildcard ./*.cpp )
HEADERS = $(wildcard ./*.h ./json/*.h)
INCLUDE = ./json
CFLAGS=-I. -I$(INCLUDE) -lpthread

OBJ = ${C_SOURCES:.cpp=.o}

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

client.out: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

