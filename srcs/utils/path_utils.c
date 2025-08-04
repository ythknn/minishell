/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:35:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/04 17:03:06 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*check_absolute_path(char *cmd)
{
	struct stat	st;

	if (access(cmd, F_OK) != 0)
	{
		print_no_such_file(cmd);
		return (NULL);
	}
	if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
	{
		print_is_directory(cmd);
		return (NULL);
	}
	if (access(cmd, X_OK) != 0)
	{
		print_permission_denied(cmd);
		return (NULL);
	}
	return (ft_strdup(cmd));
}

static char	*check_relative_path(char *cmd)
{
	struct stat	st;

	if (access(cmd, F_OK) == 0)
	{
		if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
			return (NULL);
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
	}
	return (NULL);
}

static int	is_absolute_or_relative_path(char *cmd)
{
	if (cmd[0] == '/')
		return (1);
	if (cmd[0] == '.' && cmd[1] == '/')
		return (1);
	if (cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/')
		return (1);
	return (0);
}

static char	*build_exec_path(char *dir, char *cmd)
{
	char	*exec_path;
	int		dir_len;
	int		cmd_len;
	int		i;

	if (!dir || !cmd || !*dir)
		return (NULL);
	dir_len = 0;
	while (dir[dir_len])
		dir_len++;
	cmd_len = 0;
	while (cmd[cmd_len])
		cmd_len++;
	exec_path = malloc(dir_len + cmd_len + 2);
	if (!exec_path)
		return (NULL);
	i = 0;
	while (i < dir_len)
	{
		exec_path[i] = dir[i];
		i++;
	}
	exec_path[i] = '/';
	i++;
	while (i - dir_len - 1 < cmd_len)
	{
		exec_path[i] = cmd[i - dir_len - 1];
		i++;
	}
	exec_path[i] = '\0';
	return (exec_path);
}

static void	free_paths_array(char **paths)
{
	int	i;

	i = 0;
	while (paths[i])
	{
		free(paths[i]);
		i++;
	}
	free(paths);
}

static char	*search_in_path(char *cmd, char *path_env)
{
	char	**paths;
	char	*exec_path;
	int		i;

	if (!cmd || !path_env || !*path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = -1;
	while (paths[++i])
	{
		if (paths[i] && paths[i][0])
		{
			exec_path = build_exec_path(paths[i], cmd);
			if (exec_path && access(exec_path, X_OK) == 0)
			{
				free_paths_array(paths);
				return (exec_path);
			}
			if (exec_path)
				free(exec_path);
		}
	}
	return (free_paths_array(paths), NULL);
}

char	*find_executable(char *cmd, t_shell *shell)
{
	char	*path_env;
	char	*result;

	if (!cmd || !*cmd)
		return (NULL);
	if (is_absolute_or_relative_path(cmd))
		return (check_absolute_path(cmd));
	result = check_relative_path(cmd);
	if (result)
		return (result);
	path_env = get_env_value(shell->env_list, "PATH");
	if (!path_env)
		return (NULL);
	return (search_in_path(cmd, path_env));
}
