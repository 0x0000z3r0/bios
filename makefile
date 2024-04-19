SRC=pci.c fcha50m.c
CFLAGS=-Wall -Wextra -fsanitize=leak,address,undefined

bios: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@
