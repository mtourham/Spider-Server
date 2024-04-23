NAME = webserv
 
SERVER = srcs/server

SOURCES = 	webserv.cpp\
			srcs/server/server.cpp\
			srcs/server/servers.cpp\
			srcs/parsing/Parsing.cpp\
			srcs/server/post_method.cpp\
			srcs/parsing/Parsing_utils.cpp\
			srcs/parsing/Parsing_conf.cpp\
			srcs/parsing/Parsing_loc.cpp\
			srcs/request/request.cpp\
			srcs/response/response.cpp\
			srcs/response/response_utils.cpp\
			srcs/cgi/cgi_handler.cpp

CFLAGS = -Wall -Wextra -Werror -std=c++98

${NAME} : ${SRC}
		@c++ ${CFLAGS} ${SOURCES} -o ${NAME}

all : ${NAME}

clean :
		@rm -rf ${NAME}

fclean : clean

re : fclean all