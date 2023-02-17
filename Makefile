NAME	:= webserv
INCDIR	:= inc
SRCDIR	:= src
OBJDIR	:= obj
SRC		:= $(shell find $(SRCDIR) -name '*.cpp')
OBJ		:= $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
CC		:= g++
FLAGS	= -std=c++98
# -Wall -Wextra -Werror

all: $(NAME)
$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC) -g $(FLAGS) -c $< -o $@
run: $(NAME)
	./$(NAME) Conf/test.conf
clean:
	@rm -rf $(OBJDIR)
fclean: clean
	@rm -f $(NAME)
re:	fclean all
.PHONY: clean fclean re all