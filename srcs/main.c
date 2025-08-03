/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 18:50:53 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/03 10:04:24 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/minishell.h"

int g_signal = 0;
void *g_shell_ptr = NULL;

// Atexit cleanup function - Note: This will need to be updated to use shell structure
void cleanup_on_exit(void)
{
	// This function can't access shell structure, so we'll handle cleanup in main
	// The atexit function is not ideal for this case anymore
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
	
	setup_terminal();
	setup_signals();
	init_shell(&shell, env);
	g_shell_ptr = &shell;
	while (1)
	{
		line = display_prompt(&shell);
		if (!line)
		{
			// Ctrl+D ile çıkışta shell state'i temizle
			clear_current_tokens(&shell);
			clear_current_commands(&shell);
			gc_free_all(&shell);
			break;
		}
		
		// Önce mevcut shell state'i temizle (önceki interrupt'lardan kalma olabilir)
		clear_current_tokens(&shell);
		clear_current_commands(&shell);
		
		// Ana loop değişkenlerini shell GC'ye ata (sadece line ve processed_line)
		gc_set_static_ptr(&shell, line, GC_LINE);
		
		if (*line)
		{
			add_to_history(line);
			processed_line = lexer_analyze(line);
			if (processed_line)
			{
				gc_set_static_ptr(&shell, processed_line, GC_PROCESSED_LINE);
				
				tokens = tokenize(processed_line);
				set_current_tokens(&shell, tokens); // Shell state'e ata
				if (tokens)
				{
					commands = parse(tokens);
					set_current_commands(&shell, commands); // Shell state'e ata
					if (commands)
					{
						expand_variables(commands, &shell);
						execute_commands(commands, &shell);
						clear_current_commands(&shell);
					}
					clear_current_tokens(&shell);
				}
				gc_free_type(&shell, GC_PROCESSED_LINE);
			}
		}
		gc_free_type(&shell, GC_LINE);
		
		if (g_signal == SIGINT)
		{
			// Shell parsing state'i temizle
			clear_current_tokens(&shell);
			clear_current_commands(&shell);
			
			// SIGINT durumunda tüm mevcut parsing memory'yi temizle
			gc_free_all(&shell);
			g_signal = 0;
			shell.exit_status = 130;
		}
	}
	
	// Program sonunda shell state'i temizle
	clear_current_tokens(&shell);
	clear_current_commands(&shell);
	
	// GC temizliği
	gc_free_all(&shell);
	
	free_shell(&shell);
	return (shell.exit_status);
}
