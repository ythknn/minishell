# include "../includes/minishell.h"

// ANSI color codes
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define YELLOW  "\033[33m"
# define BLUE    "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN    "\033[36m"
# define RESET   "\033[0m"

/**
 * Display the prompt and read user input
 */
char    *display_prompt(void)
{
    char    *line;
    char    *prompt;
    
    prompt = malloc(100);
    if (!prompt)
        return (NULL);
    
    sprintf(prompt, "%sminishell%s%s$>%s ", GREEN, RESET, YELLOW, RESET);
    line = readline(prompt);
    free(prompt);
    return (line);
}
