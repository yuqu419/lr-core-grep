CC      = gcc
CFLAGS  = -g -I ./include
LDFLAGS = -fsanitize=address
TARGET  = lrg
SRCS    = $(wildcard *.c ./src/*.c)          
OBJS    = $(SRCS:.c=.o)         
HDRS    = $(wildcard *.h)
PARA = $(filter-out $(GOALS),$(MAKECMDGOALS))
GOALS = all run test clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) $(PARA)
test: $(TARGET)
	./test/run_tests.sh
.PHONY: all clean run test
