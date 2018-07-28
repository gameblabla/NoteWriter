SOURCES = $(wildcard *.c)
TARGET = test.tns
OBJECTS = $(SOURCES:.c=.o)

CC = nspire-gcc
LD = nspire-ld
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -Wall -Wextra -Ofast -I. -I../
LDFLAGS =
LIBS = -lSDL -lz -lm

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET:.tns=.elf) $(TARGET)
