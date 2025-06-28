# include "../includes/minishell.h"

/**
 * Print error message to stderr with proper formatting
 */
void	print_error(char *cmd, char *arg, char *msg)
{
	ft_putstr_fd("minishell: ", 2);
	
	if (cmd)
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": ", 2);
	}
	
	if (arg)
	{
		ft_putstr_fd(arg, 2);
		ft_putstr_fd(": ", 2);
	}
	
	ft_putstr_fd(msg, 2);
	ft_putstr_fd("\n", 2);
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
