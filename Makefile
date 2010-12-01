TARGET=cuda_ga
OBJS=
SRC=cuda_ga.cu
DEP=mutex_testing/mutex_testing.h random.h cuga_ga.h ind_ga.h
CC=nvcc
CFLAGS=-g -O3 -arch sm_11
LIBS=

.PHONY:all clean
all:$(TARGET)

$(OBJS):$(DEP)

$(TARGET):$(OBJS) $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(OBJS) $(LIBS)

%.o : %.cu
	$(CC) -c -o $@ $(CFLAGS) $<

clean:
	rm -f *.o core* $(TARGET)
