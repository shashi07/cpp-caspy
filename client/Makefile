NVCC := /usr/local/cuda/bin/nvcc --ptxas-options=-v
LIBS := -L/usr/local/cuda/sdk/lib64 -L/usr/local/cuda/lib64
INCLUDE = ./json
INCS := -I/usr/local/cuda/include -I/usr/include/cuda -I./ -I/usr/local/cuda/sdk/common/inc -I. -I$(INCLUDE) -lpthread
CFLAGS := $(INCS) -c -g# -D_DEBUG
ECFLAGS := $(INCS)# -D_DEBUG
LDFLAGS := $(LIBS) -lcuda -lcudart -lrt
CC=g++
C_SOURCES = $(wildcard ./*.cpp)
HEADERS = $(wildcard ./*.h ./json/*.h)
OBJ = ${C_SOURCES:.cpp=.o}



all: client.out

hash.o: hash.cu sha1.h
	$(NVCC) $(CFLAGS) hash.cu -o hash.o

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

client.out: hash.o $(OBJ)
	$(CC) -o $@ $^ $(ECFLAGS)  $(LDFLAGS)

clean:
	rm -rf *~
	rm -rf *.o
	rm -rf *.out