CC	:= cc
CFLAGS	:= -std=c99 -O2 -Wall -Wextra -Werror=implicit-function-declaration
INCS	:= -Iinclude
SRCS	:= src/main.c src/game.c src/board.c src/input.c src/ui.c src/puzzle_loader.c src/generator.c src/config.c
OBJS	:= $(SRCS:.c=.o)

LIBS	:= -lraylib -lm -lpthread -ldl -lrt -lX11

TARGET	:= sudoku

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCS) -o $@ $^ $(LIBS)

%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCS) -c -o $@ $<

run: $(TARGET)
	./$(BIN)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean run

