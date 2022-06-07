BLUE=\033[0;38;5;123m
LIGHT_PINK = \033[0;38;5;200m
PINK = \033[0;38;5;198m
DARK_BLUE = \033[1;38;5;110m
GREEN = \033[1;32;111m
LIGHT_GREEN = \033[1;38;5;121m
LIGHT_RED = \033[0;38;5;110m
FLASH_GREEN = \033[33;32m
WHITE_BOLD = \033[37m

# nom de l'executable
NAME = ft_nm

#sources path
SRC_PATH= srcs

#objects path
OBJ_PATH= .objs

#includes
INC_PATH= includes/

HEADER=$(INC_PATH)/ft_nm.h

NAME_SRC=main.c get_shname.c utils.c print.c 32bits.c



NAME_SRC_LEN	= $(shell echo -n $(NAME_SRC) $(NAME_SRC_ASM) | wc -w)
I				= 0


OBJ_NAME		= $(NAME_SRC:.c=.o)

OBJS = $(addprefix $(OBJ_PATH)/,$(OBJ_NAME)) $(addprefix $(OBJ_ASM_PATH)/,$(OBJ_NAME_ASM))

CC			= gcc 
CFLAGS		= -Wall -Werror -Wextra


all: $(NAME)

$(NAME) : $(OBJS)
	@export LC_COLLATE=C
	@export LANG=en_US.UTF-8
	@$(CC) $^ -o $@
	@echo "	\033[2K\r$(DARK_BLUE)$(NAME):\t\t$(GREEN)loaded\033[0m"

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(HEADER)
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@$(CC) -I $(INC_PATH) -c $< -o $@
	@$(eval I=$(shell echo $$(($(I)+1))))
	@printf "\033[2K\r${G}$(DARK_BLUE)>>\t\t\t\t$(I)/$(shell echo $(NAME_SRC_LEN)) ${N}$(BLUE)$<\033[36m \033[0m"


clean:
	@rm -f $(OBJS)
	@rmdir $(OBJ_PATH) 2> /dev/null || true
	@printf "\033[2K\r$(DARK_BLUE)$(NAME) objects:\t$(LIGHT_PINK)removing\033[36m \033[0m\n"


fclean: clean
	@rm -f $(NAME)
	@printf "\033[2K\r$(DARK_BLUE)$(NAME):\t\t$(PINK)removing\033[36m \033[0m\n"

re: fclean all

.PHONY: all re clean fclean
