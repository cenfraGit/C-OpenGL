CC = gcc
CFLAGS = `pkg-config --cflags glfw3` -I/glad/include
LDFLAGS = `pkg-config --static --libs glfw3` -lGL -lm
SRCS = main.c glad/src/glad.c
TARGET = main

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
