# include "../includes/minishell.h"

/**
 * Export command implementation
 */
int	ft_export(char **args, t_shell *shell)
{
	int	 i;
	int	 j;
	char	*key;
	char	*value;

	if (!args[1])
	{
		t_env *env = shell->env_list;
		while (env)
		{
			printf("declare -x %s=\"%s\"\n", env->key, env->value);
			env = env->next;
		}
		return (0);
	}
	
	i = 1;
	while (args[i])
	{
		j = 0;
		while (args[i][j] && args[i][j] != '=')
			j++;
		
		if (args[i][j] == '=')
		{
			key = strndup(args[i], j);
			value = strdup(args[i] + j + 1);
			add_env_var(shell->env_list, key, value);
			free(key);
			free(value);
		}
		else
		{
			key = strdup(args[i]);
			if (!get_env_value(shell->env_list, key))
				add_env_var(shell->env_list, key, "");
			free(key);
		}
		i++;
	}
	
	free(shell->env_array);
	shell->env_array = env_list_to_array(shell->env_list);
	
	return (0);
}
