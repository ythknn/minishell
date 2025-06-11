# include "../includes/minishell.h"

/**
 * History management functions
 */

/**
 * Initialize history from file
 */
void    init_history(void)
{
    char    *home;
    char    *history_file;
    FILE    *fp;
    char    line[4096];

    rl_initialize();
    if (!rl_readline_name)
        rl_readline_name = "minishell";
    clear_history();
    home = getenv("HOME");
    if (!home)
    {
		return;
	}
	history_file = malloc(strlen(home) + 20);
    sprintf(history_file, "%s/.minishell_history", home);
    fp = fopen(history_file, "r");
    if (fp)
    {
        while (fgets(line, sizeof(line), fp))
        {
            line[strcspn(line, "\n")] = '\0';
            if (*line)
			add_history(line);
        }
        fclose(fp);
	}
    free(history_file);
}

/**
 * Save history to file
 */
void    save_history(void)
{
    char    *home;
    char    *history_file;
    FILE    *fp;
    HIST_ENTRY *entry;
    int     i = 0;

    home = getenv("HOME");
    if (!home)
        return;
    
    history_file = malloc(strlen(home) + 20);
    if (!history_file)
        return;
    
    sprintf(history_file, "%s/.minishell_history", home);
    
    fp = fopen(history_file, "w");
    if (fp)
    {
        while ((entry = history_get(i++)))
            fprintf(fp, "%s\n", entry->line);
        fclose(fp);
    }
    
    free(history_file);
}

/**
 * Add command to history
 */
void    add_to_history(char *line)
{
    if (line && *line)
        add_history(line);
}
