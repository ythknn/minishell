# include "../includes/minishell.h"

/**
 * Print working directory command implementation
 */
int    ft_pwd(void)
{
    char    cwd[4096];

    if (getcwd(cwd, 4096))
    {
        printf("%s\n", cwd);
        return (0);
    }
    else
    {
        print_error("pwd", NULL, strerror(errno));
        return (1);
    }
}
