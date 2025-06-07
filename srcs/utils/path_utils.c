# include "../includes/minishell.h"

/**
 * Find executable in PATH
 */
char    *find_executable(char *cmd, t_shell *shell)
{
    char    *path_env;
    char    *path;
    char    *exec_path;
    char    *token;

    if (!cmd || !*cmd)
        return (NULL);
    
    if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/') || 
        (cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/'))
    {
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
        exec_path = malloc(strlen(token) + strlen(cmd) + 2);
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
