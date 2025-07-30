# include "../includes/minishell.h"

void	init_shell(t_shell *shell, char **env)
{
	shell->env_list = create_env_list(env);
	if (!shell->env_list)
		return ;
	shell->env_array = env_list_to_array(shell->env_list);
	if (!shell->env_array)
		return ;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->processed_line = NULL;
	shell->exit_status = 0;
	shell->interactive = isatty(STDIN_FILENO);
	// Initialize GC manager to NULL
	ft_memset(&shell->gc, 0, sizeof(t_gc_manager));
}

void	free_shell(t_shell *shell)
{
	t_env	*current;
	t_env	*next;
	int		i;

	i = 0;
	if (!shell)
		return ;
	
	// Free GC-managed memory first
	gc_free_all(shell);
	
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
		while (shell->env_array[i])
			free(shell->env_array[i++]);
		free(shell->env_array);
	}
}

int ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char) s1[i] - (unsigned char) s2[i]);
}