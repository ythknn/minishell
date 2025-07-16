# include "../includes/minishell.h"

/**
 * Initialize shell structure with environment variables
 */
void	init_shell(t_shell *shell, char **env)
{
	shell->env_list = create_env_list(env);
	if (!shell->env_list)
	{
		return;
	}
	shell->env_array = env_list_to_array(shell->env_list);
	if (!shell->env_array)
	{
		return;
	}
	shell->exit_status = 0;
	shell->interactive = isatty(STDIN_FILENO);
}

void	free_shell(t_shell *shell)
{
	t_env   *current;
	t_env   *next;

	if (!shell)
		return;
	
	// Environment list'i temizle
	current = shell->env_list;
	while (current)
	{
		next = current->next;
		free(current->key);
		free(current->value);
		free(current);
		current = next;
	}
	
	if (shell->env_array)
	{
		int i = 0;
		while (shell->env_array[i])
			free(shell->env_array[i++]);
		free(shell->env_array);
	}

}
