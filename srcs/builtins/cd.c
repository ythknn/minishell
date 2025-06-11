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
			return (1);
		}
	}
	else
		path = args[1];
	
	old_pwd = getcwd(cwd, 4096);
	if (old_pwd)
		add_env_var(shell->env_list, "OLDPWD", old_pwd);
	
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		return (1);
	}
	
	if (getcwd(cwd, 4096))
		add_env_var(shell->env_list, "PWD", cwd);
	
	return (0);
}
