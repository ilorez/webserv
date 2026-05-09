# vpaths
vpath %.hpp includes
vpath %.h includes
vpath %.cpp src src/server src/request src/response src/config src/cgi src/client

NAME = webserv 

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./includes  -DDEBUG
RM = rm -rf
BUILD_DR = ./build/

HEADERS = colors.hpp container.hpp debug.hpp Request.hpp settings.hpp utils.hpp Templates.hpp \
					Client.hpp ManageClients.hpp Response.hpp Server.hpp utils.hpp WebServExceptions.hpp \
					Lexer.hpp Config.hpp serverConfig.hpp locationConfig.hpp


F_SRCS = request.cpp utils.cpp response.cpp exceptions.cpp Lexer.cpp Config.cpp locationConfig.cpp serverConfig.cpp
F_SERVER = server.cpp server_utils.cpp
F_CGI = cgi.cpp cgi_oi.cpp cgi_utils.cpp
F_CLIENT = manageClients.cpp client.cpp client_io.cpp 

SRCS = main.cpp $(F_SERVER) $(F_SRCS) $(F_CGI) $(F_CLIENT)

OBJS = $(addprefix $(BUILD_DR),$(SRCS:%.cpp=%.o))

# colors
green = \033[32m
reset = \033[0m

all: $(NAME)
	@echo "$(green)SUCCESS!!!$(reset)"

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(BUILD_DR)%.o: %.cpp $(HEADERS) | $(BUILD_DR) 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DR):
	mkdir -p $@

clean:
	$(RM) $(OBJS)
	$(RM) $(BUILD_DR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

