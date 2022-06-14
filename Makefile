CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g
VPATH = src

all: myfind.o parser.o memory.o stack.o ast.o eval_ast.o
	$(CC) $(CFALGS) -o myfind myfind.o parser.o memory.o stack.o ast.o eval_ast.o

check: myfind.o parser.o memory.o stack.o ast.o eval_ast.o
	$(CC) $(CFALGS) $(OBJS) myfind.o parser.o memory.o stack.o ast.o eval_ast.o
	sh tests/test.sh


clean: 
	rm -f *.o
	rm -f test
	rm -f myfind
	rm -f file*
