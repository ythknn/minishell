# include "../includes/minishell.h"

int g_signal = 0;

static void setup_terminal(void)
{
	struct termios term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
		return;
	term.c_lflag &= ~(ECHOCTL | ISIG);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
		return;
	
}


t_command	*parse(t_token *tokens)
{
	return (parse_token_loop(tokens));
}

int main(int argc, char **argv, char **env)
{
	t_shell shell;
	char	*line;
	char	*processed_line;
	t_token *tokens;
	t_command *commands;

	(void)argc;
	(void)argv;
	setup_terminal();
	setup_signals();
	init_shell(&shell, env);
	while (1)
	{
		line = display_prompt();
		if (!line)
			break;
		if (*line)
		{
			add_to_history(line);
			processed_line = lexer_analyze(line);
			if (processed_line)
			{
				tokens = tokenize(processed_line);
				if (tokens)
				{
					commands = parse(tokens);
					if (commands)
					{
						expand_variables(commands, &shell);
						execute_commands(commands, &shell);
						free_commands(commands);
					}
					free_tokens(tokens);
				}
				free(processed_line);
			}
		}
		free(line);
	}
	free_shell(&shell);
	return (shell.exit_status);
}
