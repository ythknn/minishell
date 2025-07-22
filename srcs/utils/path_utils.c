/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:35:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/22 17:56:18 by mdusunen         ###   ########.fr       */
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
	char	*temp;

	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	exec_path = ft_strjoin(temp, cmd);
	free(temp);
	return (exec_path);
}

static char	*search_in_path(char *cmd, char *path_env)
{
	char	*path;
	char	*token;
	char	*exec_path;

	path = ft_strdup(path_env);
	if (!path)
		return (NULL);
	token = strtok(path, ":");
	while (token)
	{
		exec_path = build_exec_path(token, cmd);
		if (!exec_path)
			return (free(path), NULL);
		if (access(exec_path, X_OK) == 0)
		{
			free(path);
			return (exec_path);
		}
		free(exec_path);
		token = strtok(NULL, ":");
	}
	free(path);
	return (NULL);
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
