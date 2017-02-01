P = play
INCLUDE = -Wall -g 
LIBS = -luv
CC = clang

r: b
	./$(P)

b: 
	$(CC) $(LIBS) $(INCLUDE) main.c -o $(P)

watch:
	find . -name '*.[c|h]' | entr -c make r
