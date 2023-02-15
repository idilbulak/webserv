NAME	= webserv
SRCS	= main.cpp \
		  Parser/Config.cpp
OBJS	= $(SRCS:cpp=o)
FLAGS	= -std=c++98 -Wall -Wextra -Werror
all		: $(NAME)
$(NAME)	: $(OBJS)
		  clang++ $(FLAGS) - o $(NAME) $(SRCS)
%.o		: %.cpp
		  clang++ $(FLAGS) -c $< -o $@
clean	: rm -rf $(OBJS)
fclean	: rm -rf $(NAME)
re		: fclean all