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
	@python3 xx/$(ERR).py
	@mv {1..5}$(ERR) xx/
run: $(NAME)
	./$(NAME) Conf/test.conf
clean:
	@rm -rf $(OBJDIR)
	@rm -rf xx/{1..5}$(ERR)
	@echo 0 > www/count.txt
	@echo "" > tester/YoupiBanane/youpi.bla
	@rm -rf tester/YoupiBanane/youpla.bla
	@echo "" > cgiBody.txt
# @rm -rf $(UPLOADDIR)
fclean: clean
	@rm -f $(NAME)
re: fclean run
.PHONY: clean fclean re all err_html
