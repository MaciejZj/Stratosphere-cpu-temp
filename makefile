CXX	:= g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb

BIN	:= bin
SRC	:= src
INCLUDE := include
LIB := lib

LIBRARIES := -lconfig++ -lzmq

EXECUTABLE := main

all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*

install:
	sudo cp ./$(BIN)/$(EXECUTABLE) /usr/bin/sp-cpu_temp
	sudo mkdir -p /etc/sp
	sudo cp sp-cpu_temp.service /etc/systemd/system/

uninstall:
	sudo rm /usr/bin/sp-cpu_temp /etc/systemd/system/sp-cpu_temp.service
