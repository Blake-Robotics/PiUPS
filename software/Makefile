C=gcc
CFLAGS = -std=gnu99 -Wall
OBJ = i2ctools.o PiUPStools.o


all:    i2ctest PiUPSStatus PiUPSConfigure PiUPSLog

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

i2ctest: $(OBJ) i2ctest.o
	gcc -o $@ $^ $(CFLAGS)

PiUPSStatus: $(OBJ) PiUPSStatus.o
	gcc -o $@ $^ $(CFLAGS)

PiUPSConfigure: $(OBJ) PiUPSConfigure.o
	gcc -o $@ $^ $(CFLAGS)
	
PiUPSLog: $(OBJ) PiUPSLog.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o i2ctest PiUPSStatus PiUPSConfigure

