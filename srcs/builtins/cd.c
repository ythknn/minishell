# include "../includes/minishell.h"

/**
 * Change directory command implementation
 */
int	ft_cd(char **args, t_shell *shell)
{
	char	*path;
	char	cwd[4096];
	char	*old_pwd;

	if (!args[1])
	{
		path = get_env_value(shell->env_list, "HOME");
		if (!path)
		{
			print_error("cd", NULL, "HOME not set");
			shell->exit_status = ERROR;
			return (ERROR);
		}
	}
	else
	{
		if (args[2])
		{
			print_error("cd", NULL, "too many arguments");
			shell->exit_status = ERROR;
			return (ERROR);
		}
		path = args[1];
	}
	
	old_pwd = getcwd(cwd, 4096);
	if (old_pwd)
		add_env_var(shell->env_list, "OLDPWD", old_pwd);
	
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		shell->exit_status = ERROR;
		return (ERROR);
	}
	
	if (getcwd(cwd, 4096))
		add_env_var(shell->env_list, "PWD", cwd);
	
	free(shell->env_array);
	shell->env_array = env_list_to_array(shell->env_list);
	shell->exit_status = SUCCESS;
	return (SUCCESS);
}
