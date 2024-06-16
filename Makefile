TARGET=a.out
CXX=C:/MinGW-SFML/bin/g++
DEBUG=-g
OPT=-O0
WARN=-Wall
INCLUDES=-IC:/SFML/include
LIBS=-LC:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
CXXFLAGS=$(DEBUG) $(OPT) $(WARN) $(INCLUDES)
LDFLAGS=$(LIBS)
OBJS=main.o tetris.o

all: $(OBJS)
	@echo "Linking..."
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Cleaning up..."
	@rm *.o
	@echo "Running..."
	@cmd /c "run.bat"

main.o: main.cpp
	@echo "Compiling main.cpp..."
	$(CXX) -c $(CXXFLAGS) main.cpp -o main.o

tetris.o: tetris.cpp
	@echo "Compiling tetris.cpp..."
	$(CXX) -c $(CXXFLAGS) tetris.cpp -o tetris.o

clean:
	@rm -f $(TARGET) *.o
