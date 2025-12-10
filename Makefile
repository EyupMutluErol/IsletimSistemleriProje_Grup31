CC = gcc
CFLAGS = -I. -I./src -I./FreeRTOS/include -I./FreeRTOS/portable/ThirdParty/GCC/Posix -Wall -Wextra -g -pthread
LDFLAGS = -pthread

SRC = src/main.c src/scheduler.c src/tasks.c \
      FreeRTOS/src/tasks.c \
      FreeRTOS/src/list.c \
      FreeRTOS/src/queue.c \
      FreeRTOS/src/timers.c \
      FreeRTOS/src/event_groups.c \
      FreeRTOS/src/stream_buffer.c \
      FreeRTOS/src/croutine.c \
      FreeRTOS/src/heap_3.c \
      FreeRTOS/portable/ThirdParty/GCC/Posix/port.c \
      FreeRTOS/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c

OBJ = $(SRC:.c=.o)

TARGET = freertos_sim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) src/*.o
