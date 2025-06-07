# include "../includes/minishell.h"

/**
 * Handle heredoc input
 */
static char *handle_heredoc(char *delimiter)
{
    char *line;
    char *content;
    char *temp;
    size_t content_size;
    int in_heredoc;
    
    content = strdup("");
    content_size = 1;
    g_signal = 0;
    in_heredoc = 1;
    
    printf("> ");
    line = readline("");
    
    while (line && strcmp(line, delimiter) != 0 && in_heredoc)
    {
        if (g_signal == SIGINT)
        {
            in_heredoc = 0;
            break;
        }
        
        content_size += strlen(line) + 1;
        temp = malloc(content_size);
        
        if (!temp)
        {
            free(content);
            free(line);
            return (NULL);
        }
        
        sprintf(temp, "%s%s\n", content, line);
        free(content);
        content = temp;
        
        free(line);
        printf("> ");
        line = readline("");
    }
    
    if (line)
        free(line);
    
    if (!in_heredoc)
    {
        free(content);
        return (NULL);
    }
    
    return (content);
}

/**
 * Setup redirections for a command
 */
int    setup_redirections(t_redir *redirs)
{
    t_redir    *current;
    int        fd;
    char       *heredoc_content;

    current = redirs;
    while (current)
    {
        if (current->type == TOKEN_REDIR_IN)
        {
            fd = open(current->file, O_RDONLY);
            if (fd == -1)
            {
                print_error("minishell", current->file, strerror(errno));
                return (1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        else if (current->type == TOKEN_REDIR_OUT)
        {
            fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                print_error("minishell", current->file, strerror(errno));
                return (1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (current->type == TOKEN_REDIR_APPEND)
        {
            fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
            {
                print_error("minishell", current->file, strerror(errno));
                return (1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (current->type == TOKEN_HEREDOC)
        {
            int pipe_fd[2];
            
            heredoc_content = handle_heredoc(current->file);
            if (!heredoc_content)
                return (1);
                
            pipe(pipe_fd);
            write(pipe_fd[1], heredoc_content, strlen(heredoc_content));
            close(pipe_fd[1]);
            dup2(pipe_fd[0], STDIN_FILENO);
            close(pipe_fd[0]);
            free(heredoc_content);
        }
        current = current->next;
    }
    return (0);
}

/**
 * Restore standard input and output
 */
void    restore_redirections(int stdin_copy, int stdout_copy)
{
    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    close(stdin_copy);
    close(stdout_copy);
}
