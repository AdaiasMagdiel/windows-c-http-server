CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lwsock32 -lws2_32
INPUT = main.c
FILES = ./src/server.c ./src/content.c
OUTPUT = main.exe

$(OUTPUT): $(INPUT) $(FILES)
	$(CC) $(CFLAGS) $(INPUT) $(FILES) -o $(OUTPUT) $(LIBS)

clean:
	rm -f $(OUTPUT)

all: $(OUTPUT)
