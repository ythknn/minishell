# include "../includes/minishell.h"

/**
 * Env command implementation
 */
int	ft_env(t_shell *shell)
{
	t_env	*env;

	env = shell->env_list;
	while (env)
	{
		printf("%s=%s\n", env->key, env->value);
		env = env->next;
	}
	
	return (0);
}
