PROG=mlw

$(PROG): main.c
	$(CC) -Wall -Wextra -static $^ -o $@

clean:
	rm $(PROG)
