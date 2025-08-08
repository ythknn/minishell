/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 20:37:58 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/08 20:37:59 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	setup_terminal(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
		return ;
	term.c_lflag &= ~(ECHOCTL | ISIG);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
		return ;
}

void	handle_signal_interrupt(t_shell *shell)
{
	clear_current_tokens(shell);
	clear_current_commands(shell);
	gc_free_all(shell);
	g_signal = 0;
	shell->exit_status = 130;
}
