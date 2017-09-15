C=gcc
CFLAGS = -std=gnu99 -Wall
OBJ = i2ctest.o 

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

i2ctest: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o i2ctest
