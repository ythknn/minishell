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
		return (NULL);
	return (search_in_path(cmd, path_env));
}

static char	*find_exec_in_path(char *cmd, char *path)
{
	char	*token;
	char	*exec_path;
	char	*saveptr;

	token = strtok_r(path, ":", &saveptr);
	while (token)
	{
		exec_path = build_exec_path(token, cmd);
		if (!exec_path)
			return (NULL);
		if (access(exec_path, X_OK) == 0)
			return (exec_path);
		free(exec_path);
		exec_path = NULL;
		token = strtok_r(NULL, ":", &saveptr);
	}
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
