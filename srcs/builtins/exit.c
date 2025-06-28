# include "../includes/minishell.h"

static int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

/**
 * Exit command implementation
 */
int	ft_exit(char **args, t_shell *shell)
{
	int	status;

	printf("exit\n");
	
	if (!args[1])
	{
		free_shell(shell);
		exit(shell->exit_status);
	}
	
	if (!is_valid_number(args[1]))
	{
		print_error("exit", args[1], "numeric argument required");
		shell->exit_status = 2;
		if (!shell->interactive)
		{
			free_shell(shell);
			exit(2);
		}
		return (2);
	}
	
	status = ft_atoi(args[1]);
	
	if (args[2])
	{
		print_error("exit", NULL, "too many arguments");
		shell->exit_status = ERROR;
		if (!shell->interactive)
		{
			free_shell(shell);
			exit(ERROR);
		}
		return (ERROR);
	}
	
	free_shell(shell);
	exit(status);
}
