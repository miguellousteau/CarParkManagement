CC = gcc
CFLAGS = -O3 -Wall -Wextra -Werror -Wno-unused-result

proj: project.c
	$(CC) $(CFLAGS) project.c -o proj

clean:
	rm -f proj