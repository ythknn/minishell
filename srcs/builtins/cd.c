/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:11:41 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/10 16:44:42 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	handle_cd_home(t_shell *shell, char **path)
{
	*path = get_env_value(shell->env_list, "HOME");
	if (!*path)
	{
		print_error("cd", NULL, "HOME not set");
		shell->exit_status = ERROR;
		return (ERROR);
	}
	return (0);
}

static int	handle_cd_oldpwd(t_shell *shell, char **path)
{
	*path = get_env_value(shell->env_list, "OLDPWD");
	if (!*path)
	{
		print_error("cd", NULL, "OLDPWD not set");
		shell->exit_status = ERROR;
		return (ERROR);
	}
	printf("%s\n", *path);
	return (0);
}

static int	handle_cd_path(char **args, t_shell *shell, char **path)
{
	if (!args[1])
		return (handle_cd_home(shell, path));
	else if (strcmp(args[1], "-") == 0)
		return (handle_cd_oldpwd(shell, path));
	else
	{
		if (args[2])
		{
			print_error("cd", NULL, "too many arguments");
			shell->exit_status = ERROR;
			return (ERROR);
		}
		*path = args[1];
	}
	return (0);
}

static int	update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	cwd[4096];

	if (old_pwd)
		add_env_var(shell->env_list, "OLDPWD", old_pwd);
	if (getcwd(cwd, 4096))
		add_env_var(shell->env_list, "PWD", cwd);
	free(shell->env_array);
	shell->env_array = env_list_to_array(shell->env_list);
	return (0);
}

int	ft_cd(char **args, t_shell *shell)
{
	char	*path;
	char	cwd[4096];
	char	*old_pwd;
	int		ret;

	ret = handle_cd_path(args, shell, &path);
	if (ret != 0)
		return (ret);
	old_pwd = getcwd(cwd, 4096);
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		shell->exit_status = ERROR;
		return (ERROR);
	}
	update_pwd_vars(shell, old_pwd);
	shell->exit_status = SUCCESS;
	return (SUCCESS);
}
