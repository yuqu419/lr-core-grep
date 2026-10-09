# ==================== 基本配置 ====================
CC      = gcc
CFLAGS  = -g -fsanitize=address -I ./include
LDFLAGS = -fsanitize=address
TARGET  = lrg
SRCS    = $(wildcard *.c ./src/*.c)          # 自动收集当前目录所有 .c 文件
OBJS    = $(SRCS:.c=.o)            # utils.c -> utils.o
HDRS    = $(wildcard *.h)
# PARA 	= -nic n data.txt
# ==================== 默认目标 ====================
all: $(TARGET)

# ==================== 链接规则 ====================
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# ==================== 编译规则 ====================
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# ==================== 清理 ====================
clean:
	rm -f $(OBJS) $(TARGET)

# ==================== 调试运行 ====================
run: $(TARGET)
	./$(TARGET) $(PARA)

check:
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem $(SRCS)
# 伪目标声明（防止与同名文件冲突）
.PHONY: all clean run check
