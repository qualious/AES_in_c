CXX := gcc
CXXFLAGS := -Wall
CXXTHEAD := -lpthread
GREEN :=\033[0;32m
NC :=\033[0m

all: finale

finale: crypto

crypto: crypto.c aes.c encode.c
	$(CXX) $(CXXFLAGS) crypto.c aes.c decode.c encode.c -o crypto

finale:
	@echo "${GREEN}Success!${NC}\nUsage:\n./crypto <-enc/-dec> <input file path> <output file path>"

clean:
	rm -f *.o *.out

