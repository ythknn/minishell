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

    //printf("DEBUG: Initializing readline\n");
    rl_initialize();
    if (!rl_readline_name)
        rl_readline_name = "minishell";
    
    //printf("DEBUG: Clearing history\n");
    clear_history();
    
    //printf("DEBUG: Getting HOME environment variable\n");
    home = getenv("HOME");
    if (!home)
    {
        //printf("DEBUG: HOME environment variable not found\n");
        return;
    }
    
    //printf("DEBUG: Creating history file path\n");
    history_file = malloc(strlen(home) + 20);
    if (!history_file)
    {
        //printf("DEBUG: Failed to allocate memory for history file path\n");
        return;
    }
    
    sprintf(history_file, "%s/.minishell_history", home);
    //printf("DEBUG: Opening history file: %s\n", history_file);
    
    fp = fopen(history_file, "r");
    if (fp)
    {
        //printf("DEBUG: Reading history file\n");
        while (fgets(line, sizeof(line), fp))
        {
            line[strcspn(line, "\n")] = '\0';
            if (*line)
            {
                //printf("DEBUG: Adding line to history: %s\n", line);
                if (add_history(line) != 0)
                {
                    //printf("DEBUG: Error adding line to history\n");
                }
            }
        }
        fclose(fp);
        //printf("DEBUG: History file read and closed\n");
    }
    else
    {
        //printf("DEBUG: Could not open history file\n");
    }
    
    free(history_file);
    //printf("DEBUG: History initialization complete\n");
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
