CC = gcc
CFLAGS = -Wall -Wextra -g -I/opt/homebrew/opt/ncurses/include
LDFLAGS = -L/opt/homebrew/opt/ncurses/lib
LIBS = -lncurses -lpanel

SRC = cdoku.c
OBJ = $(SRC:.c=.o)
TARGET = cdoku

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
