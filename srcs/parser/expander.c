# include "../includes/minishell.h"

/**
 * Expand environment variables in a string
 */
static char    *expand_env_vars(char *str, t_shell *shell)
{
    char    *result;
    char    *env_value;
    int     i;
    int     j;
    int     in_quotes;

    result = malloc(4096);
    if (!result)
        return (NULL);
    
    i = 0;
    j = 0;
    in_quotes = 0;
    
    while (str[i])
    {
        if (str[i] == '\'')
            in_quotes = !in_quotes;
        
        if (str[i] == '$' && !in_quotes)
        {
            i++;
            if (str[i] == '?')
            {
                j += sprintf(result + j, "%d", shell->exit_status);
                i++;
            }
            else if (str[i] && (str[i] == '_' || (str[i] >= 'a' && str[i] <= 'z') || 
                   (str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= '0' && str[i] <= '9')))
            {
                char var_name[256];
                int k = 0;
                
                while (str[i] && (str[i] == '_' || (str[i] >= 'a' && str[i] <= 'z') || 
                       (str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= '0' && str[i] <= '9')))
                {
                    var_name[k++] = str[i++];
                }
                var_name[k] = '\0';
                
                env_value = get_env_value(shell->env_list, var_name);
                if (env_value)
                {
                    strcpy(result + j, env_value);
                    j += strlen(env_value);
                }
            }
            else if (str[i])
            {
                result[j++] = '$';
                result[j++] = str[i++];
            }
            else
                result[j++] = '$';
        }
        else
            result[j++] = str[i++];
    }
    
    result[j] = '\0';
    return (result);
}

/**
 * Expand environment variables in command arguments
 */
void    expand_variables(t_command *cmd, t_shell *shell)
{
    t_command    *current_cmd;
    t_redir      *current_redir;
    int          i;
    char         *expanded;

    current_cmd = cmd;
    while (current_cmd)
    {
        if (current_cmd->args)
        {
            i = 0;
            while (current_cmd->args[i])
            {
                expanded = expand_env_vars(current_cmd->args[i], shell);
                free(current_cmd->args[i]);
                current_cmd->args[i] = expanded;
                i++;
            }
        }
        
        current_redir = current_cmd->redirections;
        while (current_redir)
        {
            if (current_redir->type != TOKEN_HEREDOC)
            {
                expanded = expand_env_vars(current_redir->file, shell);
                free(current_redir->file);
                current_redir->file = expanded;
            }
            current_redir = current_redir->next;
        }
        
        current_cmd = current_cmd->next;
    }
}
