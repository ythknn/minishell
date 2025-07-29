/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 18:50:53 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/28 17:29:10 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/minishell.h"

int g_signal = 0;

// Atexit cleanup function
void cleanup_on_exit(void)
{
	clear_current_tokens();
	clear_current_commands();
	gc_free_all();
}

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
	
	// Program çıkışında cleanup register et
	atexit(cleanup_on_exit);
	
	setup_terminal();
	setup_signals();
	init_shell(&shell, env);
	while (1)
	{
		line = display_prompt();
		if (!line)
		{
			// Ctrl+D ile çıkışta global state'i temizle
			clear_current_tokens();
			clear_current_commands();
			gc_free_all();
			break;
		}
		
		// Önce mevcut global state'i temizle (önceki interrupt'lardan kalma olabilir)
		clear_current_tokens();
		clear_current_commands();
		
		// Ana loop değişkenlerini GC'ye ata (sadece line ve processed_line)
		gc_set_static_ptr(line, GC_LINE);
		
		if (*line)
		{
			add_to_history(line);
			processed_line = lexer_analyze(line);
			if (processed_line)
			{
				gc_set_static_ptr(processed_line, GC_PROCESSED_LINE);
				
				tokens = tokenize(processed_line);
				set_current_tokens(tokens); // Static state'e ata
				if (tokens)
				{
					commands = parse(tokens);
					set_current_commands(commands); // Static state'e ata
					if (commands)
					{
						expand_variables(commands, &shell);
						execute_commands(commands, &shell);
						clear_current_commands();
					}
					clear_current_tokens();
				}
				gc_free_type(GC_PROCESSED_LINE);
			}
		}
		gc_free_type(GC_LINE);
		
		if (g_signal == SIGINT)
		{
			// Static parsing state'i temizle
			clear_current_tokens();
			clear_current_commands();
			
			// SIGINT durumunda tüm mevcut parsing memory'yi temizle
			gc_free_all();
			g_signal = 0;
			shell.exit_status = 130;
		}
	}
	
	// Program sonunda static state'i temizle
	clear_current_tokens();
	clear_current_commands();
	
	// GC temizliği
	gc_free_all();
	
	gc_free_all(); // Tüm GC memory'yi temizle
	free_shell(&shell);
	return (shell.exit_status);
}
