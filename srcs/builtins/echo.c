# include "../includes/minishell.h"

/**
 * Echo command implementation
 */
int	ft_echo(char **args)
{
	int	i;
	int	n_flag;

	n_flag = 0;
	i = 1;
	
	while (args[i] && strcmp(args[i], "-n") == 0)
	{
		n_flag = 1;
		i++;
	}
	
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	
	if (!n_flag)
		printf("\n");
	
	return (0);
}
