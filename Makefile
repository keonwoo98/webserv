NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic -g3

RED = \033[31m
GREEN = \033[33m
BLUE = \033[34m
BOLD = \033[1m
FAINT = \033[2m
ITALIC = \033[3m
RESET = \033[0m

################################
##           HEADERS          ##
################################
INC_DIRS = $(dir $(wildcard ./includes/*/.))
INC = $(addprefix -I, $(INC_DIRS))
# INCS = $(foreach d, $(INC_DIRS), $(wildcard $(d)*.hpp))

################################
##           SOURCES          ##
################################
SRC_DIRS = $(dir $(wildcard ./sources/*/.)) ./app/
SRCS = $(foreach d, $(SRC_DIRS), $(wildcard $(d)*.cpp))
SRC_NAMES = $(notdir $(SRCS))

################################
##           OBJECTS          ##
################################
OBJ_DIR = ./objects/
OBJ_NAMES = $(SRC_NAMES:.cpp=.o)
OBJS = $(addprefix $(OBJ_DIR), $(OBJ_NAMES))
vpath %.cpp $(SRC_DIRS)

################################
##           PHONIES          ##
################################
.PHONY: all clean fclean re info

all : $(NAME)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(RED)$(NAME) : $(OBJ_DIR) deleted$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)$(NAME) : $(NAME) deleted$(RESET)"

re:
	@$(MAKE) fclean
	@$(MAKE) all

info : 
	@echo "$(BOLD)$(ITALIC)$(BLUE)  Include paths$(RESET)"
	@echo "$(FAINT)$(ITALIC)\t$(INC_DIRS)$(RESET)" | sed 's/ /\n\t/g'

	@echo "$(BOLD)$(ITALIC)$(BLUE)  Source paths paths$(RESET)"
	@echo "$(FAINT)$(ITALIC)\t$(SRC_DIRS)$(RESET)" | sed 's/ /\n\t/g'
	
	@echo "$(BOLD)$(ITALIC)$(BLUE)  Include files$(RESET)"
	@echo "$(FAINT)$(ITALIC)\t$(INCS)$(RESET)" | sed 's/ /\n\t/g'

	@echo "$(BOLD)$(ITALIC)$(BLUE)  Source files$(RESET)"
	@echo "$(FAINT)$(ITALIC)\t$(SRCS)$(RESET)" | sed 's/ /\n\t/g'

	@echo "$(BOLD)$(ITALIC)$(BLUE)  Source file names$(RESET)"
	@echo "$(FAINT)$(ITALIC)\t$(SRC_NAMES)$(RESET)" | sed 's/ /\n\t/g'

	@echo "$(BOLD)$(ITALIC)$(BLUE)  Object files$(RESET)"
	@echo "$(FAINT)$(ITALIC)\t$(OBJS)$(RESET)" | sed 's/ /\n\t/g'

################################
##           TARGETS          ##
################################
$(NAME) : $(OBJ_DIR) $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	@echo "\n$(BLUE)$(NAME) : object files created$(RESET)"
	@echo "$(BLUE)$(NAME) : $(NAME) created$(RESET)"


	
$(OBJ_DIR) :
	@mkdir -p $@
	@echo "$(BLUE)$(NAME) : $(OBJ_DIR) created$(RESET)"
 
$(OBJ_DIR)%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INC) -c $^ -o $@
	@echo "$(BLUE).$(RESET)\c"