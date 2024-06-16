SRC = main.c socket.c retrieve.c utils.c parse.c listing.c mime.c libs.c
OBJ = $(SRC:.c=.o)

EXE=fetchmail
CC = cc
CFLAGS = -Wall

$(EXE): $(OBJ)
	cc -g -Wall -o $(EXE) $(OBJ) -lm -lssl -lcrypto

format:
	clang-format -style=file -i *.c

clean:
	rm -f $(OBJ) $(EXE)