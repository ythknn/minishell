# include "../includes/minishell.h"

/**
 * Initialize shell structure with environment variables
 */
void	init_shell(t_shell *shell, char **env)
{
	//printf("DEBUG: Creating environment list\n");
	shell->env_list = create_env_list(env);
	if (!shell->env_list)
	{
		//printf("DEBUG: Failed to create environment list\n");
		return;
	}
	//printf("DEBUG: Environment list created\n");
	
	//printf("DEBUG: Converting environment list to array\n");
	shell->env_array = env_list_to_array(shell->env_list);
	if (!shell->env_array)
	{
		//printf("DEBUG: Failed to create environment array\n");
		return;
	}
	//printf("DEBUG: Environment array created\n");
	
	shell->exit_status = 0;
	shell->interactive = 1;
	//printf("DEBUG: Shell structure initialized\n");
}

/**
 * Free shell structure and its contents
 */
void	free_shell(t_shell *shell)
{
	t_env   *current;
	t_env   *next;

	if (!shell)
		return;
	
	// Free environment list
	current = shell->env_list;
	while (current)
	{
		next = current->next;
		free(current->key);
		free(current->value);
		free(current);
		current = next;
	}
	
	// Free environment array
	if (shell->env_array)
	{
		int i = 0;
		while (shell->env_array[i])
			free(shell->env_array[i++]);
		free(shell->env_array);
	}
} 