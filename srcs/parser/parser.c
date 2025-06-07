# include "../includes/minishell.h"

/**
 * Create a new redirection node
 */
static t_redir    *create_redir(t_token_type type, char *file)
{
    t_redir    *redir;

    redir = malloc(sizeof(t_redir));
    if (!redir)
        return (NULL);
    redir->type = type;
    redir->file = strdup(file);
    redir->next = NULL;
    return (redir);
}

/**
 * Add a redirection to the end of the redirection list
 */
static void    add_redir(t_redir **redirs, t_redir *new_redir)
{
    t_redir    *current;

    if (!*redirs)
    {
        *redirs = new_redir;
        return;
    }
    current = *redirs;
    while (current->next)
        current = current->next;
    current->next = new_redir;
}

/**
 * Create a new command node
 */
static t_command    *create_command(void)
{
    t_command    *cmd;

    cmd = malloc(sizeof(t_command));
    if (!cmd)
        return (NULL);
    cmd->args = NULL;
    cmd->redirections = NULL;
    cmd->next = NULL;
    return (cmd);
}

/**
 * Add a command to the end of the command list
 */
static void    add_command(t_command **cmds, t_command *new_cmd)
{
    t_command    *current;

    if (!*cmds)
    {
        *cmds = new_cmd;
        return;
    }
    current = *cmds;
    while (current->next)
        current = current->next;
    current->next = new_cmd;
}

/**
 * Add an argument to the command's argument list
 */
static void    add_arg(t_command *cmd, char *arg)
{
    int        i;
    int        size;
    char    **new_args;

    if (!cmd->args)
    {
        cmd->args = malloc(sizeof(char *) * 2);
        cmd->args[0] = strdup(arg);
        cmd->args[1] = NULL;
        return;
    }
    
    size = 0;
    while (cmd->args[size])
        size++;
    
    new_args = malloc(sizeof(char *) * (size + 2));
    i = 0;
    while (i < size)
    {
        new_args[i] = cmd->args[i];
        i++;
    }
    new_args[i] = strdup(arg);
    new_args[i + 1] = NULL;
    
    free(cmd->args);
    cmd->args = new_args;
}

/**
 * Parse tokens into command structures
 */
t_command    *parse(t_token *tokens)
{
    t_command    *cmds;
    t_command    *current_cmd;
    t_token        *current;

    cmds = NULL;
    current_cmd = create_command();
    current = tokens;
    
    while (current)
    {
        if (current->type == TOKEN_PIPE)
        {
            add_command(&cmds, current_cmd);
            current_cmd = create_command();
        }
        else if (current->type == TOKEN_REDIR_IN || 
                 current->type == TOKEN_REDIR_OUT || 
                 current->type == TOKEN_REDIR_APPEND || 
                 current->type == TOKEN_HEREDOC)
        {
            if (current->next && current->next->type == TOKEN_WORD)
            {
                add_redir(&current_cmd->redirections, 
                          create_redir(current->type, current->next->value));
                current = current->next;
            }
        }
        else if (current->type == TOKEN_WORD)
        {
            add_arg(current_cmd, current->value);
        }
        
        current = current->next;
    }
    
    add_command(&cmds, current_cmd);
    return (cmds);
}

/**
 * Free all commands in the command list
 */
void    free_commands(t_command *cmds)
{
    t_command    *current_cmd;
    t_command    *next_cmd;
    t_redir        *current_redir;
    t_redir        *next_redir;
    int            i;

    current_cmd = cmds;
    while (current_cmd)
    {
        next_cmd = current_cmd->next;
        
        if (current_cmd->args)
        {
            i = 0;
            while (current_cmd->args[i])
            {
                free(current_cmd->args[i]);
                i++;
            }
            free(current_cmd->args);
        }
        
        current_redir = current_cmd->redirections;
        while (current_redir)
        {
            next_redir = current_redir->next;
            free(current_redir->file);
            free(current_redir);
            current_redir = next_redir;
        }
        
        free(current_cmd);
        current_cmd = next_cmd;
    }
}
