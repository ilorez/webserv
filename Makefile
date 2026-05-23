# vpaths
vpath %.hpp includes
vpath %.h includes
vpath %.cpp src src/server src/request src/response src/config src/cgi src/client src/cookies

NAME = webserv 

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./includes  -DDEBUG # -g
RM = rm -rf
BUILD_DR = ./build/

HEADERS = CGIClient.hpp Client.hpp colors.hpp Config.hpp container.hpp debug.hpp EpollHold.hpp Lexer.hpp locationConfig.hpp ManageClients.hpp Request.hpp Response.hpp serverConfig.hpp Server.hpp Session.hpp sessionManager.hpp settings.hpp Templates.hpp utils.hpp WebServExceptions.hpp

F_SRCS = exceptions.cpp utils.cpp
F_SERVER = Server.cpp ServerUtils.cpp 
F_CGI = Cgi.cpp CgiIO.cpp CgiUtils.cpp
F_CLIENT = Client.cpp ClientIO.cpp ManageClients.cpp 
F_CONFIG = Config.cpp Lexer.cpp locationConfig.cpp serverConfig.cpp
F_COOKIES = Session.cpp sessionManager.cpp
F_REQUEST = request.cpp request_utils.cpp
F_RESPONSE = response.cpp response_utils.cpp

SRCS = main.cpp $(F_SRCS) $(F_SERVER) $(F_CGI) $(F_CLIENT) $(F_CONFIG) $(F_COOKIES) $(F_REQUEST) $(F_RESPONSE)

OBJS = $(addprefix $(BUILD_DR),$(SRCS:%.cpp=%.o))

# colors
green = \033[32m
reset = \033[0m

all: $(NAME) $(CONFIG)
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

