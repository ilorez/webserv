CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -g
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = program

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)


re: fclean all

.PHONY: all clean fclean re
