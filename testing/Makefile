TARGET=c_ga
OBJS=main.o c_ga.o
DEP=c_ga.h
CFLAGS=-g -Wall
LIBS=

.PHONY:all clean
all:$(TARGET)

$(OBJS):$(DEP)

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LIBS)

clean:
	rm -f *.o core* $(TARGET)
