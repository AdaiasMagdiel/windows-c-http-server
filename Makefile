CC = gcc
SRC = main.c
TARGET = server.exe
CLIBS = -lWs2_32
CFLAGS = -Werror -Wall -Wextra

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(CLIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)

.PHONY: rebuild
rebuild: clean all
