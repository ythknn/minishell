/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:35:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/15 18:35:01 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*find_executable(char *cmd, t_shell *shell)
{
	char		*path_env;
	char		*path;
	char		*exec_path;
	char		*token;
	struct stat	st;

	if (!cmd || !*cmd)
		return (NULL);
	if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/')
		|| (cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/'))
	{
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
		return (strdup(cmd));
	}
	if (access(cmd, F_OK) == 0)
	{
		if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
		{
			return (NULL);
		}
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd));
		return (NULL);
	}
	path_env = get_env_value(shell->env_list, "PATH");
	if (!path_env)
		return (NULL);
	path = strdup(path_env);
	if (!path)
		return (NULL);
	token = strtok(path, ":");
	while (token)
	{
		exec_path = malloc(ft_strlen(token) + ft_strlen(cmd) + 2);
		if (!exec_path)
		{
			free(path);
			return (NULL);
		}
		sprintf(exec_path, "%s/%s", token, cmd);
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
