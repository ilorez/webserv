# vpaths
vpath %.hpp includes
vpath %.h includes
vpath %.cpp src src/server src/request src/response src/config src/cgi src/client src/cookies

NAME = webserv 

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./includes  -DDEBUG # -g
RM = rm -rf
BUILD_DR = ./build/

HEADERS = CGIClient.hpp Client.hpp Colors.hpp Config.hpp container.hpp Debug.hpp EpollHold.hpp Lexer.hpp LocationConfig.hpp ManageClients.hpp Request.hpp Response.hpp ServerConfig.hpp Server.hpp Session.hpp SessionManager.hpp Settings.hpp Templates.hpp Utils.hpp WebServExceptions.hpp

F_SRCS = exceptions.cpp Utils.cpp
F_SERVER = Server.cpp ServerUtils.cpp 
F_CGI = Cgi.cpp CgiIO.cpp CgiUtils.cpp
F_CLIENT = Client.cpp ClientIO.cpp ManageClients.cpp 
F_CONFIG = Config.cpp Lexer.cpp LocationConfig.cpp ServerConfig.cpp
F_COOKIES = Session.cpp SessionManager.cpp
F_REQUEST = Request.cpp RequestUtils.cpp
F_RESPONSE = Response.cpp ResponseUtils.cpp

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

