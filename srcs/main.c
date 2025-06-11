# include "../includes/minishell.h"

int g_signal = 0;

static void setup_terminal(void)
{
    struct termios term;

    //printf("DEBUG: Setting up terminal\n");
    if (tcgetattr(STDIN_FILENO, &term) == -1)
    {
        //printf("DEBUG: Error getting terminal attributes\n");
        return;
    }
    
    term.c_lflag &= ~(ECHOCTL | ISIG);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
    {
        //printf("DEBUG: Error setting terminal attributes\n");
        return;
    }
    //printf("DEBUG: Terminal setup complete\n");
}

int main(int argc, char **argv, char **env)
{
    t_shell shell;
    char    *line;
    char    *processed_line;
    t_token *tokens;
    t_command *commands;

    (void)argc;
    (void)argv;
    
    //printf("DEBUG: Starting program\n");
    setup_terminal();
    setup_signals();
    
    //printf("DEBUG: Initializing shell\n");
    init_shell(&shell, env);
    //printf("DEBUG: Shell initialized\n");
    
    //printf("DEBUG: Initializing history\n");
    init_history();
    //printf("DEBUG: History initialized\n");
    
    while (1)
    {
        //printf("DEBUG: Displaying prompt\n");
        line = display_prompt();
        if (!line)
        {
            //printf("DEBUG: No input received, exiting\n");
            break;
        }
        
        if (*line)
        {
            //printf("DEBUG: Processing line: %s\n", line);
            add_to_history(line);
            processed_line = lexer_analyze(line);
            if (processed_line)
            {
                //printf("DEBUG: Line analyzed\n");
                tokens = tokenize(processed_line);
                if (tokens)
                {
                    //printf("DEBUG: Tokens created\n");
                    commands = parse(tokens);
                    if (commands)
                    {
                        //printf("DEBUG: Commands parsed\n");
                        expand_variables(commands, &shell);
                        //printf("DEBUG: Variables expanded\n");
                        shell.exit_status = execute_commands(commands, &shell);
                        //printf("DEBUG: Commands executed\n");
                        free_commands(commands);
                    }
                    free_tokens(tokens);
                }
                free(processed_line);
            }
        }
        free(line);
    }
    //printf("DEBUG: Saving history\n");
    save_history();
    //printf("DEBUG: Freeing shell\n");
    free_shell(&shell);
    //printf("DEBUG: Program finished\n");
    return (shell.exit_status);
}
