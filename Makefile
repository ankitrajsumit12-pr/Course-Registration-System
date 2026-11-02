# Makefile for Course Registration & Seat Allocation System
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = course_system.exe
SRCS = main.c course_system.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c course_system.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
