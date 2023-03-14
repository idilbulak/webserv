NAME		:= webserv
INCDIR  	:= inc
SRCDIR  	:= src
OBJDIR  	:= obj
SRC     	:= $(shell find $(SRCDIR) -name '*.cpp')
OBJ     	:= $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
HEAD    	:= $(wildcard inc/*.hpp)
CC      	:= g++
FLAGS   	:= -std=c++98
ERR   		:= xx_html
UPLOADDIR	:= uploadDir
# -Wall -Wextra -Werror

all: $(NAME) 
$(NAME): $(OBJ) $(ERR)
	@mkdir -p $(@D)
	@$(CC) $(FLAGS) $(OBJ) -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEAD)
	@mkdir -p $(@D)
	@$(CC) -g $(FLAGS) -c $< -o $@
$(ERR):
	@mkdir -p {1..5}$(ERR)
# @mkdir -p $(UPLOADDIR)
	@python3 assets/$(ERR).py
run: $(NAME)
	./$(NAME) Conf/test.conf
clean:
	@rm -rf $(OBJDIR)
	@rm -rf {1..5}$(ERR)
	@echo 0 > assets/cgi-bin/count.txt
# @rm -rf $(UPLOADDIR)
fclean: clean
	@rm -f $(NAME)
re: fclean all
.PHONY: clean fclean re all err_html
