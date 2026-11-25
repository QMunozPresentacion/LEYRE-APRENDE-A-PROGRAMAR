CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O0 -g -fno-stack-protector -no-pie
LDFLAGS = -no-pie
OBJ = main.o ui.o usuarios.o gatos.o

gatos: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

$(OBJ): config.h ui.h usuarios.h gatos.h

clean:
	rm -f $(OBJ) gatos

.PHONY: clean