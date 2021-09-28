CXX = g++
CXXFLAGS = -Iheaders -MMD -MP -c
        
# Rule to link object code files to create executable file
jumpgame2: jumpgame2.o
  $(CXX) jumpgame2.o -lSDL2 -o jumpgame2

# Rules to compile source code file to object code
jumpgame2.o: jumpgame2.cpp
	$(CXX) $(CXXFLAGS) -o jumpgame2.o jumpgame2.cpp
   
# Pseudo-target to remove object code and executable files
clean:
	rm -f jumpgame2 jumpgame2.o

# g++ -Iheaders -MMD -MP -c -o jumpgame2.o jumpgame2.cpp
# g++ jumpgame2.o -lSDL2 -o jumpgame2
