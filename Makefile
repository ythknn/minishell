# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/07 14:33:48 by yihakan           #+#    #+#              #
#    Updated: 2025/08/08 21:01:20 by yihakan          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


GREEN	= \033[32m
YELLOW	= \033[33m
BLUE	= \033[34m
MAGENTA	= \033[35m
CYAN	= \033[36m
RESET	= \033[0m

# Banner
BANNER	= \
	$(CYAN) ╔═══════════════════════════════════════════════════════════════════════╗$(RESET)\n\
	$(CYAN)║$(RESET)                                                                       $(CYAN)║$(RESET)\n\
	$(CYAN)║$(RESET)  $(MAGENTA)███╗   ███╗██╗███╗   ██╗██╗███████╗██╗  ██╗███████╗██╗     ██╗     $(CYAN)  ║$(RESET)\n\
	$(CYAN)║$(RESET)  $(MAGENTA)████╗ ████║██║████╗  ██║██║██╔════╝██║  ██║██╔════╝██║     ██║     $(CYAN)  ║$(RESET)\n\
	$(CYAN)║$(RESET)  $(MAGENTA)██╔████╔██║██║██╔██╗ ██║██║███████╗███████║█████╗  ██║     ██║     $(CYAN)  ║$(RESET)\n\
	$(CYAN)║$(RESET)  $(MAGENTA)██║╚██╔╝██║██║██║╚██╗██║██║╚════██║██╔══██║██╔══╝  ██║     ██║     $(CYAN)  ║$(RESET)\n\
	$(CYAN)║$(RESET)  $(MAGENTA)██║ ╚═╝ ██║██║██║ ╚████║██║███████║██║  ██║███████╗███████╗███████╗$(CYAN)  ║$(RESET)\n\
	$(CYAN)║$(RESET)  $(MAGENTA)╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝$(CYAN)  ║$(RESET)\n\
	$(CYAN)║$(RESET)                                                                       $(CYAN)║$(RESET)\n\
	$(CYAN)║$(RESET)  $(YELLOW)by yihakan & mdusunen$(RESET)                                                $(CYAN)║$(RESET)\n\
	$(CYAN)║$(RESET)                                                                       $(CYAN)║$(RESET)\n\
	$(CYAN)╚═══════════════════════════════════════════════════════════════════════╝$(RESET)\n

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -f

# Directories
SRCS_DIR = srcs
INCLUDES_DIR = includes
LIBFT_DIR = libft
OBJ_DIR = obj

# Source files
SRCS = $(SRCS_DIR)/main.c \
	   $(SRCS_DIR)/main_utils.c \
	   $(SRCS_DIR)/prompt/prompt.c \
	   $(SRCS_DIR)/prompt/history.c \
	   $(SRCS_DIR)/parser/lexer.c \
	   $(SRCS_DIR)/parser/tokenizer.c \
	   $(SRCS_DIR)/parser/tokenizer_utils.c \
	   $(SRCS_DIR)/parser/parser.c \
	   $(SRCS_DIR)/parser/parser_2.c \
	   $(SRCS_DIR)/parser/parser_3.c \
	   $(SRCS_DIR)/parser/parser_utils.c \
	   $(SRCS_DIR)/parser/expander.c \
	   $(SRCS_DIR)/parser/expander_utils.c \
	   $(SRCS_DIR)/executor/executor.c \
	   $(SRCS_DIR)/executor/redirections.c \
	   $(SRCS_DIR)/executor/redirections_io.c \
	   $(SRCS_DIR)/executor/redirections_heredoc.c \
	   $(SRCS_DIR)/executor/heredoc_setup.c \
	   $(SRCS_DIR)/executor/heredoc_core.c \
	   $(SRCS_DIR)/executor/pipes.c \
	   $(SRCS_DIR)/executor/pipe_utils.c \
	   $(SRCS_DIR)/executor/pipe_handlers.c \
	   $(SRCS_DIR)/builtins/builtins.c \
	   $(SRCS_DIR)/builtins/echo.c \
	   $(SRCS_DIR)/builtins/cd.c \
	   $(SRCS_DIR)/builtins/pwd.c \
	   $(SRCS_DIR)/builtins/export.c \
	   $(SRCS_DIR)/builtins/export_utils.c \
	   $(SRCS_DIR)/builtins/unset.c \
	   $(SRCS_DIR)/builtins/env.c \
	   $(SRCS_DIR)/builtins/exit.c \
	   $(SRCS_DIR)/builtins/pills.c \
	   $(SRCS_DIR)/builtins/pills_utils.c \
	   $(SRCS_DIR)/utils/env_utils.c \
	   $(SRCS_DIR)/utils/env_utils_2.c \
	   $(SRCS_DIR)/utils/env_utils_3.c \
	   $(SRCS_DIR)/utils/path_utils.c \
	   $(SRCS_DIR)/utils/path_utils_2.c \
	   $(SRCS_DIR)/utils/error_utils.c \
	   $(SRCS_DIR)/utils/error_utils_2.c \
	   $(SRCS_DIR)/utils/shell_utils.c \
	   $(SRCS_DIR)/utils/gc_manager.c \
	   $(SRCS_DIR)/utils/gc_init_utils.c \
	   $(SRCS_DIR)/utils/gc_manager_utils.c \
	   $(SRCS_DIR)/utils/static_vars.c \
	   $(SRCS_DIR)/utils/static_vars_2.c \
	   $(SRCS_DIR)/signals/signals.c

# Object files
OBJS = $(patsubst $(SRCS_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Libraries
READLINE_INCLUDE = -I/opt/homebrew/opt/readline/include
READLINE_LIB = -L/opt/homebrew/opt/readline/lib
LIBFT = $(LIBFT_DIR)/libft.a
LIBS = -L$(LIBFT_DIR) -lft -lreadline

# Include paths
INCLUDES = -I$(INCLUDES_DIR) -I$(LIBFT_DIR) $(READLINE_INCLUDE)

# Rules
all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	@echo "$(BANNER)"
	@echo "$(GREEN)Compiling minishell...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) $(READLINE_LIB) $(LIBS) -o $(NAME)
	@echo "$(GREEN)minishell compiled successfully!$(RESET)"

$(LIBFT):
	@echo "$(YELLOW)Compiling libft...$(RESET)"
	@make -C $(LIBFT_DIR)
	@echo "$(GREEN)libft compiled successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/prompt
	@mkdir -p $(OBJ_DIR)/parser
	@mkdir -p $(OBJ_DIR)/executor
	@mkdir -p $(OBJ_DIR)/builtins
	@mkdir -p $(OBJ_DIR)/utils
	@mkdir -p $(OBJ_DIR)/signals
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(YELLOW)Cleaning object files...$(RESET)"
	@$(RM) -r $(OBJ_DIR)
	@make -C $(LIBFT_DIR) clean
	@echo "$(GREEN)Clean completed!$(RESET)"

fclean: clean
	@echo "$(YELLOW)Removing executable...$(RESET)"
	@$(RM) $(NAME)
	@make -C $(LIBFT_DIR) fclean
	@echo "$(GREEN)Fclean completed!$(RESET)"

re: fclean all

leaks:
	valgrind --show-leak-kinds=all --leak-check=full --suppressions=readline.supp ./minishell

.PHONY: all clean fclean re
