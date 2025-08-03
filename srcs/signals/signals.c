/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:36:07 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/03 10:04:37 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// void free_heredoc(t_shell *shell)
// {
// 	static t_shell *temp_shell;

// 	if (shell)
// 	{
// 		temp_shell = shell;
// 	}
// 	else
// 	{
// 		free_shell(shell);
// 	}
// }

void	handle_sigint(int sig)
{
	(void)sig;
	g_signal = SIGINT;
	
	// Static parsing state'i temizle
	if (g_shell_ptr)
	{
		clear_current_tokens((t_shell *)g_shell_ptr);
		clear_current_commands((t_shell *)g_shell_ptr);
	}
	
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

void	handle_heredoc_sigint(int sig)
{
	(void)sig;
	g_signal = SIGINT;
	
	// Static parsing state'i temizle
	if (g_shell_ptr)
	{
		clear_current_tokens((t_shell *)g_shell_ptr);
		clear_current_commands((t_shell *)g_shell_ptr);
		
		// Heredoc interrupt'ında tüm parsing memory'yi temizle
		gc_free_all((t_shell *)g_shell_ptr);
	}
	rl_replace_line("", 0);
	rl_done = 1;
}

void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_heredoc_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

void	reset_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
