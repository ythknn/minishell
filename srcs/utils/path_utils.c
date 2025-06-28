# include "../includes/minishell.h"

/**
 * Find executable in PATH
 */
char	*find_executable(char *cmd, t_shell *shell)
{
	char	*path_env;
	char	*path;
	char	*exec_path;
	char	*token;
	struct stat	st;

	if (!cmd || !*cmd)
		return (NULL);
	
	if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/') || 
		(cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/'))
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
			// For directories in current directory without explicit path, bash treats them as "command not found"
			// Only print "Is a directory" for explicit paths
			return (NULL);
		}
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd));
		// For files in current directory without explicit path, bash treats them as "command not found"
		// Only print permission denied for explicit paths
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
		exec_path = malloc(ft_strlen(token) + ft_strlen(cmd) + 2);		if (!exec_path)
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
