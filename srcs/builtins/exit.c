# include "../includes/minishell.h"

/**
 * Exit command implementation
 */
int    ft_exit(char **args, t_shell *shell)
{
    int    status;

    printf("exit\n");
    
    if (!args[1])
    {
        free_shell(shell);
        exit(shell->exit_status);
    }
    
    status = atoi(args[1]);
    
    if (args[2])
    {
        print_error("exit", NULL, "too many arguments");
        return (1);
    }
    
    free_shell(shell);
    exit(status);
}
