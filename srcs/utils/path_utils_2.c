/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 20:27:16 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/09 20:51:46 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
		path_env = "/usr/local/bin:/usr/bin:/bin";
	return (search_in_path(cmd, path_env));
}

static char	*find_exec_in_path(char *cmd, char *path)
{
	char	**path_array;
	char	*exec_path;
	int		i;

	if (!path || !*path)
		return (NULL);
	path_array = ft_split(path, ':');
	if (!path_array)
		return (NULL);
	i = 0;
	while (path_array[i])
	{
		exec_path = build_exec_path(path_array[i], cmd);
		if (exec_path && access(exec_path, X_OK) == 0)
		{
			free_args(path_array);
			return (exec_path);
		}
		if (exec_path)
			free(exec_path);
		i++;
	}
	free_args(path_array);
	return (NULL);
}

char	*search_in_path(char *cmd, char *path_env)
{
	char	*path;
	char	*result;

	if (!cmd || !path_env)
		return (NULL);
	path = ft_strdup(path_env);
	if (!path)
		return (NULL);
	result = find_exec_in_path(cmd, path);
	free(path);
	return (result);
}
