/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:11:41 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/26 18:24:15 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	handle_cd_path(char **args, t_shell *shell, char **path)
{
	if (!args[1])
	{
		*path = get_env_value(shell->env_list, "HOME");
		if (!*path)
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
			shell->exit_status = SUCCESS;
			return (SUCCESS);
		}
		*path = args[1];
	}
	return (0);
}

int	ft_cd(char **args, t_shell *shell)
{
	char	*path;
	char	cwd[4096];
	char	*old_pwd;
	int		ret;
	int		i;

	ret = handle_cd_path(args, shell, &path);
	if (ret != 0)
		return (ret);
	old_pwd = getcwd(cwd, 4096);
	if (old_pwd)
		shell->env_list = add_env_var(shell->env_list, "OLDPWD", old_pwd);
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		shell->exit_status = ERROR;
		return (ERROR);
	}
	if (getcwd(cwd, 4096))
		shell->env_list = add_env_var(shell->env_list, "PWD", cwd);
	if (shell->env_array)
	{
		i = 0;
		while (shell->env_array[i])
			free(shell->env_array[i++]);
		free(shell->env_array);
	}
	shell->env_array = env_list_to_array(shell->env_list);
	shell->exit_status = SUCCESS;
	return (SUCCESS);
}