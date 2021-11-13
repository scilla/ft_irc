SRCS			= srcs/main.cpp
OBJ				= $(SRCS:.cpp=.o)

CC				= clang++
RM				= rm -f
CFLAGS			= -std=c++98 -g  #-fsanitize=address -Wall -Wextra -Werror
NAME			= ircserv

%.o:			%.cpp
				@$(CC) $(CFLAGS) -c $< -o $@

all:			$(NAME)
stl:			$(NAME_STL)

$(NAME_STL):	clean $(OBJ)
				@$(CC) $(CFLAGS) $(CFLAGS_STL) -o $(NAME_STL) $(OBJ)

$(NAME):		$(OBJ)
				@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
				@$(RM) $(OBJ)

fclean:			clean
				@$(RM) $(NAME) $(NAME_STL)

re:				fclean $(NAME)

g:				fclean $(OBJS)
				$(CC) -g -o $(NAME) ${SRCS} ${LIBS} 

.PHONY:			all clean fclean re g
