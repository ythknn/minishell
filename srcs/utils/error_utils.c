# include "../includes/minishell.h"

// ANSI color codes
# define RED	 "\033[31m"
# define GREEN   "\033[32m"
# define YELLOW  "\033[33m"
# define BLUE	"\033[34m"
# define MAGENTA "\033[35m"
# define CYAN	"\033[36m"
# define RESET   "\033[0m"

/**
 * Print error message to stderr with proper formatting
 */
void	print_error(char *cmd, char *arg, char *msg)
{
	fprintf(stderr, "%sminishell%s: ", RED, RESET);
	
	if (cmd)
		fprintf(stderr, "%s%s%s: ", YELLOW, cmd, RESET);
	
	if (arg)
		fprintf(stderr, "%s%s%s: ", CYAN, arg, RESET);
	
	fprintf(stderr, "%s%s%s\n", RED, msg, RESET);
}

/**
 * Print syntax error message
 */
void	print_syntax_error(char *token)
{
	print_error("syntax error near unexpected token", token, "");
}

/**
 * Print command not found error
 */
void	print_command_not_found(char *cmd)
{
	print_error(cmd, NULL, "command not found");
}

/**
 * Print permission denied error
 */
void	print_permission_denied(char *path)
{
	print_error(path, NULL, "Permission denied");
}

/**
 * Print no such file or directory error
 */
void	print_no_such_file(char *path)
{
	print_error(path, NULL, "No such file or directory");
}

/**
 * Print is a directory error
 */
void	print_is_directory(char *path)
{
	print_error(path, NULL, "Is a directory");
}
