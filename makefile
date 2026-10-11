CC      = gcc
CFLAGS  = -g -I ./include
LDFLAGS = -fsanitize=address
TARGET  = lrg
SRCS    = $(wildcard *.c ./src/*.c)          
OBJS    = $(SRCS:.c=.o)         
HDRS    = $(wildcard *.h)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) $(PARA)
.PHONY: all clean run
