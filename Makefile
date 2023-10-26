INC :=  
LIB := -lm -lpthread

CC := gcc
CFLAGS := -g3 -Wall -D_DBG_CREATE_PROCESS

TARGET := paramisp
OBJ := paramisp.o md5.o

$(TARGET):$(OBJ)  
	$(CC) $(INC) $(CFLAGS) $(LIB) $(OBJ)  -Wl,-Map,$(TARGET).map -o $@
  
.SUFFIXES: .c .o .cpp  

.cpp.o:  
	$(CC) $(CFLAGS) $(INC) -c $*.cpp -o $*.o  

.c.o:  
	$(CC) $(CFLAGS) $(INC) -c $*.c -o $*.o  

test:
	./$(TARGET)

.PRONY:clean  
clean:  
	@echo "Removing linked and compiled files......"  
	rm -f $(OBJ) $(TARGET) $(TARGET).map core

