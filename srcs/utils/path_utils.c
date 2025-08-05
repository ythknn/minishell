/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:35:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/03 10:43:15 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*check_absolute_path(char *cmd)
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

char	*check_relative_path(char *cmd)
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

int	is_absolute_or_relative_path(char *cmd)
{
	if (cmd[0] == '/')
		return (1);
	if (cmd[0] == '.' && cmd[1] == '/')
		return (1);
	if (cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/')
		return (1);
	return (0);
}

char	*build_exec_path(char *dir, char *cmd)
{
	char	*exec_path;
	char	*temp;

	if (!dir || !cmd)
		return (NULL);
	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	exec_path = ft_strjoin(temp, cmd);
	free(temp);
	if (!exec_path)
		return (NULL);
	return (exec_path);
}
