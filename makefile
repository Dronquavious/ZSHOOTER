CXX = g++
CXXFLAGS = -std=c++17 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

main: src/main.cpp
	$(CXX) $(CXXFLAGS) src/*.cpp -o src/main $(LDFLAGS)

run: main
	@./src/main

clean:
	rm -f src/main
