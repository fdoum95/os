# In  order  to  execute  this  "Makefile " just  type  "make "
OBJS     = main.o semaphores.o SharedMemory.o
SOURCE   = main.cpp semaphores.cpp SharedMemory.cpp
HEADER   = semaphores.h SharedMemory.h
OUT      = ask
CC       = g++
FLAGS    = -g -c
# -g  option  enables  debugging  mode
# -c flag  generates  object  code  for  separate  files
all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)
#  create/ compile  the  individual  files  >> separately <<

main.o: main.cpp $(HEADER)
	$(CC) $(FLAGS) main.cpp

semaphores.o: semaphores.cpp semaphores.h
	$(CC) $(FLAGS) semaphores.cpp

SharedMemory.o: SharedMemory.cpp SharedMemory.h
	$(CC) $(FLAGS) SharedMemory.cpp

#  clean  house
clean :
	rm -f $(OBJS) $(OUT)
