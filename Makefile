# Makefile for wuziqi_ai
# 使用项目内 deps/sdl2 目录，无需外部依赖
# 前提：系统需安装 MinGW GCC 并确保 gcc 在 PATH 中

CC = gcc
CFLAGS = -Wall -std=c99 -O2
INCLUDES = -Iinclude -Ideps/sdl2/include
LDFLAGS = -Ldeps/sdl2/lib -static-libgcc -static-libstdc++
LIBS = -lmingw32 -lSDL2main -lSDL2 -mwindows

SRCS = main.c src/board.c src/ai.c src/render.c src/event.c src/save_load.c
OBJS = main.o src/board.o src/ai.o src/render.o src/event.o src/save_load.o
TARGET = wuziqi_ai.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

src/board.o: src/board.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

src/ai.o: src/ai.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

src/render.o: src/render.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

src/event.o: src/event.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

src/save_load.o: src/save_load.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	del /Q $(OBJS) $(TARGET) 2>nul

rebuild: clean all

.PHONY: all clean rebuild
