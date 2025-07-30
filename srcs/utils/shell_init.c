/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:30:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/30 17:50:44 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	setup_terminal(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
		return ;
	term.c_lflag &= ~(ECHOCTL | ISIG);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
		return ;
}

void	init_minishell(t_shell *shell, char **env)
{
	atexit(cleanup_on_exit);
	setup_terminal();
	setup_signals();
	init_shell(shell, env);
}

void	handle_sigint_cleanup(t_shell *shell)
{
	clear_current_tokens();
	clear_current_commands();
	gc_free_all(shell);
	g_signal = 0;
	shell->exit_status = 130;
}

void	process_tokens(char *processed_line, t_shell *shell)
{
	t_token		*tokens;
	t_command	*commands;

	tokens = tokenize(processed_line);
	set_current_tokens(tokens);
	if (tokens)
	{
		commands = parse(tokens);
		set_current_commands(commands);
		if (commands)
		{
			expand_variables(commands, shell);
			execute_commands(commands, shell);
			clear_current_commands();
		}
		clear_current_tokens();
	}
}

void	process_lexed_line(char *processed_line, t_shell *shell)
{
	if (processed_line)
	{
		gc_set_static_ptr(shell, processed_line, GC_PROCESSED_LINE);
		process_tokens(processed_line, shell);
		gc_free_type(shell, GC_PROCESSED_LINE);
	}
}
