CC=g++
EXE=test

INC=-I/usr/local/include `pkg-config --cflags opencv`
LIBFLAG=-L/usr/local/lib `pkg-config --libs opencv`

SRC_OBJ := $(patsubst %.cpp,%.o,$(wildcard ${EXE}.cpp))
SRC_BIN := $(patsubst %.cpp,%,$(wildcard ${EXE}.cpp))

.PHONY:all
all:Obj bin

.PHONY:Obj
Obj: $(SRC_OBJ)
$(SRC_OBJ): %.o :%.cpp
	$(CC) -c $(INC) $< -o $@

.PHONY:bin
bin:$(EXE)
$(EXE):$(SRC_OBJ)
	$(CC) $(INC)  $^ -o $@  $(LIBFLAG)
	rm -f $^

.PHONY:clean
clean:
	rm -f *.o $(EXE)

.PHONY:install
install:
	sudo cp ../*.h /usr/local/include