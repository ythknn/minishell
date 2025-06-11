# include "../includes/minishell.h"

/**
 * Unset command implementation
 */
int	ft_unset(char **args, t_shell *shell)
{
	int	i;

	if (!args[1])
		return (0);
	
	i = 1;
	while (args[i])
	{
		remove_env_var(&shell->env_list, args[i]);
		i++;
	}
	
	free(shell->env_array);
	shell->env_array = env_list_to_array(shell->env_list);
	
	return (0);
}
