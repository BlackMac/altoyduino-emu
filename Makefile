CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lSDL -lSDLmain -framework Cocoa

OBJ = altoyduino.o eeprom.o port.o cpu.o timer.o display.o loader.o usart.o

altoyduino: $(OBJ)
	$(CC) $(CFLAGS) -o altoyduino $(OBJ) $(LDFLAGS)

%.o : src/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o altoyduino
